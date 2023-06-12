package tram.dsa

import chisel3._
import chisel3.util._
import scala.collection.mutable


/** Controlled Register with Affine Access (Write) Pattern
 * 
 * @param width          register data width
 * @param lgMaxWI        log2(max writing Interval)
 * @param lgMaxLat       log2(max starting latency)
 * @param lgMaxCycles    log2(max writing cycles)
 * @param lgMaxRepeats   log2(max repeat number)
 */
class AffineCtrlReg(width: Int, lgMaxWI: Int, lgMaxLat: Int, lgMaxCycles: Int, lgMaxRepeats: Int) extends Module {
  val cfgWidth = width + lgMaxWI + lgMaxLat + lgMaxCycles + lgMaxRepeats + 1 // width : Initialized value; 1 : if accept io.in at the first cycle
  val io = IO(new Bundle {
    val start = Input(Bool()) // pulse signal, should be valid before latency 0, namely -1
    val bypass = Input(Bool()) // in -> reg -> out
    val config = Input(UInt(cfgWidth.W))
    val in = Input(UInt(width.W))
    val out = Output(Vec(2, UInt(width.W)))
  })

  val valueReg = RegInit(0.U(width.W))

  // Config elements
  // [name, (id, high-bit, low-bit)]
  val cfg_idx: mutable.Map[String, (Int, Int, Int)] = mutable.Map()
  // io.config should keep constant during io.en is true
  var offset = 0
  var id = 0
  val initVal = io.config(width+offset-1, offset)
  cfg_idx += "InitVal" -> (id, width+offset-1, offset)
  offset += width
  id += 1
  val WI = io.config(lgMaxWI+offset-1, offset)
  cfg_idx += "WI" -> (id, lgMaxWI+offset-1, offset)
  offset += lgMaxWI
  id += 1
  val latency = io.config(lgMaxLat+offset-1, offset) // the latency of starting input or output
  cfg_idx += "Latency" -> (id, lgMaxLat+offset-1, offset)
  offset += lgMaxLat
  id += 1
  val cycles = io.config(lgMaxCycles+offset-1, offset)
  cfg_idx += "Cycles"-> (id, lgMaxCycles+offset-1, offset)
  offset += lgMaxCycles
  id += 1
  val repeats = io.config(lgMaxRepeats+offset-1, offset)
  cfg_idx += "Repeats"-> (id, lgMaxRepeats+offset-1, offset)
  offset += lgMaxRepeats
  id += 1
  val skipFirst = io.config(offset, offset).asBool
  cfg_idx += "SkipFirst"-> (id, offset, offset)
  offset += 1
  id += 1

  val s_idle :: s_pre_lat :: s_data :: Nil = Enum(3)
  val state = RegInit(s_idle)
  val wiCnt = RegInit(0.U(lgMaxWI.W))
  val latCnt = RegInit(0.U(lgMaxLat.W))
  val cycleCnt = RegInit(0.U(lgMaxCycles.W))
  val repeatCnt = RegInit(0.U(lgMaxRepeats.W))
  val wiEnd = (wiCnt+1.U >= WI)
  val cycleCntEnd = (cycleCnt+1.U >= cycles)
  val repeatCntEnd = (repeatCnt+1.U >= repeats)

  switch(state){
    is(s_idle){
      latCnt := 0.U
      when(io.start && latency === 0.U){
        state := s_data
      }.elsewhen(io.start && latency =/= 0.U){
        state := s_pre_lat
      }
    }
    is(s_pre_lat){ // the latency before starting to write
      when(latCnt + 1.U >= latency){
        state := s_data
      }
      latCnt := latCnt + 1.U
    }
    is(s_data){
      when(repeatCntEnd && cycleCntEnd && wiEnd){
        state := s_idle
      }
    }
  }

  val launch = (state === s_data)
  when(state === s_idle){
    wiCnt := 0.U
  }.elsewhen(launch){
    wiCnt := Mux(wiEnd, 0.U, wiCnt+1.U)
  }

  when(state === s_idle){
    cycleCnt := 0.U
  }.elsewhen(launch && wiEnd){
    cycleCnt := Mux(cycleCntEnd, 0.U, cycleCnt + 1.U)
  }

  when(state === s_idle){
    repeatCnt := 0.U
  }.elsewhen(launch && wiEnd && cycleCntEnd){
    repeatCnt := repeatCnt + 1.U
  }

  when(io.bypass || (launch && wiCnt === 0.U && cycleCnt > 0.U)){
    valueReg := io.in
  }.elsewhen(launch && wiCnt === 0.U && cycleCnt === 0.U && !skipFirst){
    valueReg := io.in
  }.elsewhen(launch && wiCnt === 0.U && cycleCnt === 0.U && skipFirst){
    valueReg := initVal
  }
  io.out(0) := valueReg

  val valueRegAcc = RegInit(0.U(width.W))
  when(state === s_idle){
    valueRegAcc := initVal
  }.elsewhen(launch && wiCnt === 0.U && cycleCnt === 0.U && skipFirst){
    valueRegAcc := initVal
  }.elsewhen(launch && wiCnt === 0.U && cycleCntEnd && !skipFirst){
    valueRegAcc := initVal
  }.elsewhen(launch && wiCnt === 0.U){
    valueRegAcc := io.in
  }
  io.out(1) := valueRegAcc
}


/** Dual-mode Register supporting Affine Access (Write) Pattern and simple reg
  *
  * @param width          register data width
  * @param isAffine       is Affine Access (Write) Pattern
  * @param lgMaxWI        log2(max writing Interval)
  * @param lgMaxLat       log2(max starting latency)
  * @param lgMaxCycles    log2(max writing cycles)
  * @param lgMaxRepeats   log2(max repeat number)
  */
class DualModeReg(width: Int, isAffine: Boolean, lgMaxWI: Int, lgMaxLat: Int, lgMaxCycles: Int, lgMaxRepeats: Int) extends Module {
  val cfgWidth = {
    if(isAffine){
      width + lgMaxWI + lgMaxLat + lgMaxCycles + lgMaxRepeats + 1 // width : Initialized value; 1 : if accept io.in at the first cycle
    }else{
      0
    }
  }
  val numOut = { if(isAffine) 2 else 1 }
  val io = IO(new Bundle {
    val start = Input({if(isAffine) Bool() else UInt(0.W)}) // pulse signal, should be valid before latency 0, namely -1
    val bypass = Input({if(isAffine) Bool() else UInt(0.W)}) // in -> reg -> out
    val config = Input(UInt(cfgWidth.W))
    val in = Input(UInt(width.W))
    val out = Output(Vec(numOut, UInt(width.W)))
  })

  // Config elements
  // [name, (id, high-bit, low-bit)]
  val cfg_idx: mutable.Map[String, (Int, Int, Int)] = mutable.Map()

  if(isAffine){
    val acr = Module(new AffineCtrlReg(width, lgMaxWI, lgMaxLat, lgMaxCycles, lgMaxRepeats))
    acr.io.start := io.start
    acr.io.bypass := io.bypass
    acr.io.config := io.config
    acr.io.in := io.in
    io.out := acr.io.out
    cfg_idx ++= acr.cfg_idx
  }else{
    io.out(0) := RegNext(io.in)
  }

}


// object VerilogGen extends App {
////   (new chisel3.stage.ChiselStage).emitVerilog(new AffineCtrlReg(32, 16, 8, 16, 16),args)
//   (new chisel3.stage.ChiselStage).emitVerilog(new DualModeReg(32, true, 16, 8, 16, 16),args)
// }
