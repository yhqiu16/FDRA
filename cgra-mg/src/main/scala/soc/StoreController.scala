package tram

import chisel3._
import chisel3.util._
import freechips.rocketchip.tile._
import freechips.rocketchip.config._
import freechips.rocketchip.rocket._
import TramISA._

/** Store controller, transfer data from local scratchpad (physical address) to remote memory (virtual address)
  * @param spadAddrWidth  scratchpad address width, <= 32
  * @param lgMaxDataLen   log2(the max data length in one DMA request)
  * @param dataWidth      data width in bits
  * @param hasMask        if has mask signal in the stream interface
  * @param idWidth        width of the ID for a DMA request
  * @param streamQueDepth stream queue depth, can be zero
  */
class StoreController(spadAddrWidth: Int, lgMaxDataLen: Int, dataWidth: Int, hasMask: Boolean, idWidth: Int, streamQueDepth: Int)
                    (implicit p: Parameters) extends CoreModule {
  val io = IO(new Bundle {
    val core = new CoreIF(idWidth)
    val dma = Flipped(new DMAStreamWriteIF(lgMaxDataLen, dataWidth, hasMask, idWidth))
    val spad = Flipped(new SpadStreamReadIF(spadAddrWidth, lgMaxDataLen, dataWidth, hasMask, idWidth))
  })

  val dataByte = dataWidth/8
  val s_idle :: s_spad_req :: s_dma_req :: s_stream :: s_exp :: s_resp :: Nil = Enum(6)
  val state = RegInit(s_idle)
  val remote_addr = RegInit(0.U(coreMaxAddrBits.W)) // remote DRAM virtual address
  val spad_addr = RegInit(0.U(spadAddrWidth.W)) // scratchpad address from rs1
  val leftLen = RegInit(0.U(lgMaxDataLen.W)) // left data length
  val status = Reg(new MStatus)
  val id = RegInit(0.U(idWidth.W))
  val success = RegInit(true.B)
  val rs1 = io.core.req.bits.cmd.rs1.asTypeOf(new StoreRs1)
  val rs2 = io.core.req.bits.cmd.rs2.asTypeOf(new StoreRs2)

  switch(state){
    is(s_idle){
      when(io.core.req.fire){
        state := s_spad_req
        remote_addr := rs1.remote_addr
        spad_addr := rs2.spad_addr
        leftLen := rs2.total_len
        status := io.core.req.bits.cmd.status
        id := io.core.req.bits.id
        success := true.B
      }
    }
    is(s_spad_req){
      when(io.spad.req.fire){
        state := s_dma_req
      }
    }
    is(s_dma_req){
      when(io.dma.req.fire){
        state := s_stream
      }
    }
    is(s_stream){
      when(io.dma.exp.req){
        state := s_exp
        success := false.B
      }.elsewhen(io.spad.stream.fire){
        leftLen := leftLen - dataByte.U
        state := Mux(leftLen <= dataByte.U, s_resp, s_stream)
      }
    }
    is(s_exp){
      when(io.spad.exp.ack){
        state := s_resp
      }
    }
    is(s_resp){
      when(io.core.resp.fire){
        state := s_idle
      }
    }
  }

  io.core.req.ready := (state === s_idle)
  // Scratchpad request
  io.spad.req.valid := (state === s_spad_req)
  io.spad.req.bits.addrs(0) := spad_addr
  io.spad.req.bits.len := leftLen
  io.spad.req.bits.id := id
  // DMA request
  io.dma.req.valid := (state === s_dma_req)
  io.dma.req.bits.addr := remote_addr
  io.dma.req.bits.len := leftLen
  io.dma.req.bits.id := id
  io.dma.req.bits.status := status
  // data stream
  if(streamQueDepth == 0){
    io.spad.stream.ready := io.dma.stream.ready && (state === s_stream)
    io.dma.stream.valid := io.spad.stream.valid && (state === s_stream)
    io.dma.stream.bits := io.spad.stream.bits
  }else{
    val que = Module(new Queue(new DMAStream(dataWidth, hasMask, idWidth), streamQueDepth,
      false, false, false, true))
    io.spad.stream.ready := que.io.enq.ready && (state === s_stream)
    que.io.enq.valid := io.spad.stream.valid && (state === s_stream)
    que.io.enq.bits := io.spad.stream.bits
    io.dma.stream <> que.io.deq
    que.io.flush.get := io.spad.exp.req
  }
  // exception
  io.spad.exp.req := (state === s_exp)
  io.spad.exp.id := id
  io.dma.exp.ack := io.spad.exp.ack
  // response
  io.core.resp.valid := (state === s_resp)
  io.core.resp.bits.success := success
  io.core.resp.bits.id := id
}