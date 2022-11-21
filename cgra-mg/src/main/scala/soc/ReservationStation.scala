package tram

import chisel3._
import chisel3.util._
import freechips.rocketchip.tile._
import freechips.rocketchip.config._
import freechips.rocketchip.rocket._
import TramISA._

import scala.collection.immutable


class CoreReq(idWidth: Int)(implicit p: Parameters) extends Bundle {
  val cmd = new RoCCCommand
  val id = UInt(idWidth.W)
}

class CoreResp(idWidth: Int)(implicit p: Parameters) extends Bundle {
  val success = Bool()
  val id = UInt(idWidth.W)
}

class CoreIF(idWidth: Int)(implicit p: Parameters) extends Bundle {
  val req = Flipped(Decoupled(new CoreReq(idWidth)))
  val resp = Decoupled(new CoreResp(idWidth))
}

/** Reservation Station
  * @param loadQueDepth   load queue depth
  * @param storeQueDepth  store queue depth
  * @param exeQueDepth    execute queue depth
  * @param idWidth        width of the command ID
  */
class ReservationStation(loadQueDepth: Int, storeQueDepth: Int, exeQueDepth: Int, idWidth: Int)
                    (implicit p: Parameters) extends CoreModule {
  val io = IO(new Bundle {
    val cmd = Flipped(Decoupled(new RoCCCommand))
    val resp = Decoupled(new RoCCResponse)
    val ld = Flipped(new CoreIF(idWidth))
    val st = Flipped(new CoreIF(idWidth))
    val ex = Flipped(new CoreIF(idWidth))
    val busy = Output(Bool()) // has command not completed
    val flush = Output(Bool()) // flush TLB
  })
  // id is composed of sub_task_id and task_id.
  val taskIdWidth = TramISA.TASK_ID_WIDTH
  assert(idWidth > taskIdWidth)

//  val cmd_que = Module(new Queue(new RoCCCommand, cmdQueDepth))
//  cmd_que.io.enq <> io.cmd
  // command queue
  val ld_cmd_que = Module(new Queue(new RoCCCommand, loadQueDepth))
  val st_cmd_que = Module(new Queue(new RoCCCommand, storeQueDepth))
  val ex_cmd_que = Module(new Queue(new RoCCCommand, exeQueDepth))
  // reservation queue to track if command is completed
  val ld_dat_resv_que = Module(new Queue(UInt(taskIdWidth.W), loadQueDepth)) // load data
  val ld_cfg_resv_que = Module(new Queue(UInt(taskIdWidth.W), 3)) // load cfg
  val st_resv_que = Module(new Queue(UInt(taskIdWidth.W), storeQueDepth))
  val ex_resv_que = Module(new Queue(UInt((1+taskIdWidth).W), exeQueDepth)) // +1: identify cfg or exe command

  val s_idle :: s_enq :: s_fence :: s_flush :: Nil = Enum(4)
  val state = RegInit(s_idle)
  val funct = io.cmd.bits.inst.funct
  val isLoadCmd = (funct === LOAD_CMD)
  val isCfgCmd = (funct === CFG_CMD)
  val isExeCmd = (funct === EXE_CMD)
  val isStoreCmd = (funct === STORE_CMD)
  val isFlushCmd = (funct === FLUSH_CMD)
  val isFenceCmd = (funct === FENCE_CMD)
  val flush = RegInit(false.B)
  val req_rd = Reg(chiselTypeOf(io.resp.bits.rd))
  val req_xd = RegInit(false.B)
  val cmd_ready = RegInit(false.B)

  io.flush := flush
  io.cmd.ready := cmd_ready
  io.busy := (state =/= s_idle) || io.cmd.valid
  switch(state) {
    is(s_idle) {
      when(io.cmd.valid){
        when(isFlushCmd) {
          state := s_flush
          req_rd := io.cmd.bits.inst.rd
          cmd_ready := true.B
          flush := true.B
        }.elsewhen(isFenceCmd && !ld_cmd_que.io.deq.valid && !ex_cmd_que.io.deq.valid && !st_cmd_que.io.deq.valid && 
                  !ld_dat_resv_que.io.deq.valid && !ld_cfg_resv_que.io.deq.valid && !ex_resv_que.io.deq.valid && !st_resv_que.io.deq.valid){ // previous commands have all been completed
          state := s_fence
          req_rd := io.cmd.bits.inst.rd
          req_xd := io.cmd.bits.inst.xd
          cmd_ready := true.B
        }.elsewhen((isLoadCmd && ld_cmd_que.io.enq.ready) || // push to load command queue
                   ((isCfgCmd || isExeCmd) && ex_cmd_que.io.enq.ready) ||  // push to execute command queue
                   (isStoreCmd && st_cmd_que.io.enq.ready)){ // push to store command queue
          state := s_enq
          cmd_ready := true.B
//          req.cmd := cmd_que.io.deq.bits
//          req.id := task_id
//        }.elsewhen(isExeCmd && ex_resv_que.io.enq.ready && // && !st_resv_que.io.deq.valid, previous store commands have all been completed
//          (!ld_resv_que.io.deq.valid || task_id =/= ld_resv_que.io.deq.bits)){ // load commands with the same task id have been completed
//          state := s_enq
//          cmd_ready := true.B
////          req.cmd := cmd_que.io.deq.bits
////          req.id := task_id
//        }.elsewhen(isStoreCmd && st_resv_que.io.enq.ready && (!ex_resv_que.io.deq.valid || task_id =/= ex_resv_que.io.deq.bits)){ // execute commands with the same task id have been completed
//          state := s_enq
//          cmd_ready := true.B
////          req.cmd := cmd_que.io.deq.bits
////          req.id := task_id
        }
      }
    }
    is(s_enq){
      cmd_ready := false.B
      state := s_idle
//      when((isLoadReq && io.ld.req.ready) ||
//           (isExeReq && io.ex.req.ready) ||
//           (isStoreReq && io.st.req.ready)){
//        state := s_idle
//      }
    }
    is(s_fence){
      cmd_ready := false.B
      when(!req_xd || io.resp.ready){
        state := s_idle
      }
    }
    is(s_flush){
      cmd_ready := false.B
      when(io.resp.ready){
        state := s_idle
        flush := false.B
      }
    }
  }

  io.resp.valid := (state === s_flush) || (state === s_fence && req_xd)
  io.resp.bits.rd := req_rd
  io.resp.bits.data := 1.U

  // command enque
  ld_cmd_que.io.enq.valid := io.cmd.fire && isLoadCmd
  ld_cmd_que.io.enq.bits := io.cmd.bits
  ex_cmd_que.io.enq.valid := io.cmd.fire && (isCfgCmd || isExeCmd)
  ex_cmd_que.io.enq.bits := io.cmd.bits
  st_cmd_que.io.enq.valid := io.cmd.fire && isStoreCmd
  st_cmd_que.io.enq.bits := io.cmd.bits

  val s_wait :: s_issue :: Nil = Enum(2)
  val ld_state = RegInit(s_wait)
  val ex_state = RegInit(s_wait)
  val st_state = RegInit(s_wait)
  val ld_cmd_task_id = ld_cmd_que.io.deq.bits.rs2.asTypeOf(new LoadRs2).task_id
  val ex_cmd_task_id = ex_cmd_que.io.deq.bits.rs2.asTypeOf(new ExeRs2).task_id // cfg and exe command have the same format
  val st_cmd_task_id = st_cmd_que.io.deq.bits.rs2.asTypeOf(new StoreRs2).task_id
  val ld_cmd_dep = ld_cmd_que.io.deq.bits.rs2.asTypeOf(new LoadRs2).dep_type
  val ex_cmd_dep = ex_cmd_que.io.deq.bits.rs2.asTypeOf(new ExeRs2).dep_type // cfg and exe command have the same format
  val is_ld_cfg = ld_cmd_que.io.deq.bits.rs2.asTypeOf(new LoadRs2).is_cfg
  val last_ld_cmd_task_id = RegInit(0.U(ld_cmd_task_id.getWidth.W))
  val ex_resv_task_id = ex_resv_que.io.deq.bits(taskIdWidth-1, 0)
  val ex_resv_is_cfg = ex_resv_que.io.deq.bits(taskIdWidth)
  val ld_resv_que_ready = (!is_ld_cfg && ld_dat_resv_que.io.enq.ready) || (is_ld_cfg && ld_cfg_resv_que.io.enq.ready)

  switch(ld_state){
    is(s_wait){
      when(ld_cmd_que.io.deq.valid){
        when((ld_cmd_task_id === ex_cmd_task_id || !ex_cmd_que.io.deq.valid) &&  // the same task id or no execute command
             (ld_cmd_task_id === st_cmd_task_id || !st_cmd_que.io.deq.valid)){   // the same task id or no store command
          when((ld_cmd_dep === LD_DEP_ST_LAST_TASK || ld_cmd_dep === LD_DEP_ST_LAST_SEC_TASK) && st_resv_que.io.deq.valid){ // depends on the store commands of the previous one/two tasks
            ld_state := s_wait                                                          // wait until previous store commands are completed
          }.elsewhen(ld_cmd_dep === LD_DEP_EX_LAST_TASK && ex_resv_que.io.deq.valid &&
                    (!is_ld_cfg || ex_resv_is_cfg)) { // depends on the cfg or execute commands of the previous task
            ld_state := s_wait
          }.otherwise{
            ld_state := s_issue
          }
        }.elsewhen((ld_cmd_task_id === ex_cmd_task_id || !ex_cmd_que.io.deq.valid) &&  // the same task id or no execute command
                   (ld_cmd_task_id =/= st_cmd_task_id && st_cmd_que.io.deq.valid) &&
                   (ld_cmd_dep =/= LD_DEP_ST_LAST_TASK)) {   // different task id
          when(ld_cmd_dep === LD_DEP_ST_LAST_SEC_TASK && st_resv_que.io.deq.valid) { // depends on the store commands of the previous second task
            ld_state := s_wait // wait until previous store commands are completed
          }.elsewhen(ld_cmd_dep === LD_DEP_EX_LAST_TASK && ex_resv_que.io.deq.valid &&
                    (!is_ld_cfg || ex_resv_is_cfg)) { // depends on the cfg or execute commands of the previous task
            ld_state := s_wait
          }.otherwise {
            ld_state := s_issue
          }
        }.elsewhen(ex_cmd_que.io.deq.valid && ld_cmd_task_id =/= ex_cmd_task_id &&
                   st_cmd_que.io.deq.valid && ex_cmd_task_id === st_cmd_task_id){
          when(ld_cmd_dep === LD_DEP_ST_LAST_TASK || ld_cmd_dep === LD_DEP_EX_LAST_TASK) { // depends on the store/execute commands of the previous task
            ld_state := s_wait
          }.elsewhen(ld_cmd_task_id =/= last_ld_cmd_task_id && ex_cmd_task_id =/= last_ld_cmd_task_id){
            ld_state := s_wait
          }.elsewhen(ld_cmd_dep === LD_DEP_ST_LAST_SEC_TASK && st_resv_que.io.deq.valid){ // depends on the store commands of the last second task
            ld_state := s_wait
          }.otherwise{
            ld_state := s_issue
          }
        }
      }
    }
    is(s_issue){
      when(io.ld.req.ready && ld_resv_que_ready){
        ld_state := s_wait
        last_ld_cmd_task_id := ld_cmd_task_id
      }
    }
  }

  val is_cfg_cmd = ex_cmd_que.io.deq.bits.inst.funct === CFG_CMD
  switch(ex_state){
    is(s_wait){
      when(ex_cmd_que.io.deq.valid && (!st_cmd_que.io.deq.valid || ex_cmd_task_id === st_cmd_task_id) &&
        (!ld_cmd_que.io.deq.valid || ex_cmd_task_id =/= ld_cmd_task_id)){
        when(ex_cmd_dep === EX_DEP_ST_LAST_TASK && st_resv_que.io.deq.valid) { // depends on the store commands of the previous task
          ex_state := s_wait // wait until previous store commands are completed
        }.elsewhen((is_cfg_cmd && ld_cfg_resv_que.io.deq.valid && ex_cmd_task_id === ld_cfg_resv_que.io.deq.bits) || // wait until previous load-cfg commands are completed
          (!is_cfg_cmd && ld_dat_resv_que.io.deq.valid && ex_cmd_task_id === ld_dat_resv_que.io.deq.bits)) { // wait until previous load-data commands are completed
          ex_state := s_wait
        }.otherwise {
          ex_state := s_issue
        }
      }
    }
    is(s_issue){
      when(io.ex.req.ready && ex_resv_que.io.enq.ready){
        ex_state := s_wait
      }
    }
  }

  switch(st_state){
    is(s_wait){
      when(st_cmd_que.io.deq.valid && (!ex_resv_que.io.deq.valid || st_cmd_task_id =/= ex_resv_que.io.deq.bits)){ // wait until previous execute commands are completed
        when((!ld_cmd_que.io.deq.valid || st_cmd_task_id =/= ld_cmd_task_id) &&
             (!ex_cmd_que.io.deq.valid || st_cmd_task_id =/= ex_cmd_task_id)) { //
          st_state := s_issue
        }
      }
    }
    is(s_issue){
      when(io.st.req.ready && st_resv_que.io.enq.ready){
        st_state := s_wait
      }
    }
  }

  ld_cmd_que.io.deq.ready := (ld_state === s_issue) && io.ld.req.ready && ld_resv_que_ready
  ld_dat_resv_que.io.enq.valid := (ld_state === s_issue) && io.ld.req.ready && (!is_ld_cfg)
  ld_dat_resv_que.io.enq.bits := ld_cmd_task_id
  ld_cfg_resv_que.io.enq.valid := (ld_state === s_issue) && io.ld.req.ready && is_ld_cfg
  ld_cfg_resv_que.io.enq.bits := ld_cmd_task_id
  io.ld.req.valid := (ld_state === s_issue) && ld_resv_que_ready
  io.ld.req.bits.cmd := ld_cmd_que.io.deq.bits
  io.ld.req.bits.id := Cat(is_ld_cfg, ld_cmd_task_id)
  io.ld.resp.ready := true.B
  ld_dat_resv_que.io.deq.ready := io.ld.resp.fire && (!io.ld.resp.bits.id(taskIdWidth).asBool)
  ld_cfg_resv_que.io.deq.ready := io.ld.resp.fire && io.ld.resp.bits.id(taskIdWidth).asBool
//  ld_resv_que.io.deq.bits === io.ld.resp.bits.id // verification

  ex_cmd_que.io.deq.ready := (ex_state === s_issue) && io.ex.req.ready && ex_resv_que.io.enq.ready
  ex_resv_que.io.enq.valid := (ex_state === s_issue) && io.ex.req.ready
  ex_resv_que.io.enq.bits := ex_cmd_task_id
  io.ex.req.valid := (ex_state === s_issue) && ex_resv_que.io.enq.ready
  io.ex.req.bits.cmd := ex_cmd_que.io.deq.bits
  io.ex.req.bits.id := ex_cmd_task_id
  io.ex.resp.ready := true.B
  ex_resv_que.io.deq.ready := io.ex.resp.fire

  st_cmd_que.io.deq.ready := (st_state === s_issue) && io.st.req.ready && st_resv_que.io.enq.ready
  st_resv_que.io.enq.valid := (st_state === s_issue) && io.st.req.ready
  st_resv_que.io.enq.bits := st_cmd_task_id
  io.st.req.valid := (st_state === s_issue) && st_resv_que.io.enq.ready
  io.st.req.bits.cmd := st_cmd_que.io.deq.bits
  io.st.req.bits.id := st_cmd_task_id
  io.st.resp.ready := true.B
  st_resv_que.io.deq.ready := io.st.resp.fire

}