package tram.dsa

import chisel3._
import chisel3.util._
import scala.collection.mutable
import scala.collection.mutable.ArrayBuffer
import tram.common.MacroVar._




/** I/O Controller
 * @param dataWidth      data width in bits
 * @param addrWidth      address width (+1 every data width)
 * @param hasMask        if has write data byte mask
 * @param mode           0: FIFO mode (with din, dout), 1: SRAM mode (with addr, din, dout)
 * @param lgMaxII        log2(max in/out Initialization Interval)
 * @param lgMaxLat       log2(max in/out latency)
 * @param lgMaxStride    log2(max address stride, n represent n*dataWidth bits)
 * @param lgMaxCycles    log2(max in/out cycles)
 * @param agNestLevels   nested levels of the address generation
 * @param addRegSram     write/read latency, 0 : 0/1; 1 : 1/2; 2 : 1/3; for both FIFO and SRAM modes
 */
class IOController(dataWidth: Int, addrWidth: Int, hasMask: Boolean, mode: Int, lgMaxII: Int,
                   lgMaxLat: Int, lgMaxStride: Int, lgMaxCycles: Int, agNestLevels: Int, addRegSram: Int) extends Module {
  val numIn = { if(mode == FIFO_MODE) 1 else 2 }
  val cfgWidth = addrWidth + (lgMaxStride + lgMaxCycles) * agNestLevels + lgMaxII + lgMaxLat + 1 + { if(mode == FIFO_MODE) 0 else 1 }
  // ----------- base_addr --- stride --- cycles ---- latency ----- II ---- isStore ----- useAddr -----
  val io = IO(new Bundle{
    val sram = Flipped(new SRAMIO(dataWidth, addrWidth, hasMask))
    val start = Input(Bool()) // pulse signal, should be valid before latency 0, namely -1
    val done = Output(Bool()) // transfer done, keep true until next start
    val config = Input(UInt(cfgWidth.W))
    val in = Vec(numIn, Input(UInt(dataWidth.W))) // FIFO: 0->data; SRAM: 0->data, 1->address in bytes
    val out = Vec(1, Output(UInt(dataWidth.W)))
  })

  val dataByte = dataWidth/8
  val lgDataByte = log2Ceil(dataByte)
  assert(dataWidth%8 == 0 && isPow2(dataWidth/8))
  val weWidth = {if(hasMask) dataByte else 1}

  // IO Controller config
  // [name, (id, high-bit, low-bit)]
  val ioc_cfg_idx: mutable.Map[String, (Int, Int, Int)] = mutable.Map()
  // io.config should keep constant during io.en is true
  var offset = 0
  var id = 0
  val baseAddr = io.config(addrWidth+offset-1, offset)
  ioc_cfg_idx += "BaseAddr" -> (id, addrWidth+offset-1, offset)
  offset += addrWidth
  id += 1
  val stride = new ArrayBuffer[SInt]()
  val cycles = new ArrayBuffer[UInt]()
  for(i <- 0 until agNestLevels){
    stride += io.config(lgMaxStride+offset-1, offset).asSInt // address stride
    ioc_cfg_idx += ("Stride" + i.toString) -> (id, lgMaxStride+offset-1, offset)
    offset += lgMaxStride
    cycles += io.config(lgMaxCycles+offset-1, offset)
    ioc_cfg_idx += ("Cycles" + i.toString) -> (id+1, lgMaxCycles+offset-1, offset)
    offset += lgMaxCycles
    id += 2
  }
  val II = io.config(lgMaxII+offset-1, offset)
  ioc_cfg_idx += "II" -> (id, lgMaxII+offset-1, offset)
  offset += lgMaxII
  id += 1
  val latency = io.config(lgMaxLat+offset-1, offset) // the latency of starting input or output
  ioc_cfg_idx += "Latency" -> (id, lgMaxLat+offset-1, offset)
  offset += lgMaxLat
  id += 1
  val isStore = io.config(offset).asBool // whether input data is valid or output data is valid
  ioc_cfg_idx += "IsStore" -> (id, offset, offset)
  val useAddr = {
    if(mode == FIFO_MODE) {
      false.B
    } else {
      ioc_cfg_idx += "UseAddr" -> (id+1, offset+1, offset+1)
      io.config(offset+1).asBool // whether use io.in(1) as address
    }
  }

  val s_idle :: s_pre_lat :: s_data :: Nil = Enum(3)
  val state = RegInit(s_idle)
  val latCnt = RegInit(0.U(lgMaxLat.W))
  val cycleCnt = Seq.fill(agNestLevels){RegInit(0.U(lgMaxCycles.W))}
  val iiCnt = RegInit(0.U(lgMaxII.W))
  val done = RegInit(false.B)
  val iiEnd = (iiCnt+1.U >= II)
  val cycleCntEnd = (0 until agNestLevels).map{ i =>
    (0 to i).map{ j => cycleCnt(j) + 1.U >= cycles(j) }.reduce(_&_)
  }
  val cntEnd = cycleCntEnd(agNestLevels-1)
//  val cntEnd = cycleCntEnd.reduce(_ & _)
  io.done := done
  switch(state){
    is(s_idle){
      latCnt := 0.U
      when(io.start && latency === 0.U){
        state := s_data
        done := false.B
      }.elsewhen(io.start && latency =/= 0.U){
        state := s_pre_lat
        done := false.B
      }
    }
    is(s_pre_lat){ // the latency before starting input or output
      when(latCnt + 1.U >= latency){
        state := s_data
      }
      latCnt := latCnt + 1.U
    }
    is(s_data){
      when(cntEnd && iiEnd){
        state := s_idle
        done := true.B
      }
    }
  }

  val launch = (state === s_data)
  when(state === s_idle){
    iiCnt := 0.U
  }.elsewhen(launch){
    iiCnt := Mux(iiEnd, 0.U, iiCnt+1.U)
  }

  for(i <- 0 until agNestLevels){
    when(state === s_idle){
      cycleCnt(i) := 0.U
    }.elsewhen(launch){
      when(iiEnd && !cycleCntEnd(i)){
        cycleCnt(i) := cycleCnt(i) + {
          if(i == 0) 1.U // add 1 every II cycle
          else cycleCntEnd(i-1)
        }
      }.elsewhen(iiEnd){
        cycleCnt(i) := 0.U
      }
    }
  }

  // write real valid: the last level with stride = 0 and cycleCntEnd
  val wRealValid = (0 until agNestLevels).map{ i =>
      (stride(i) === 0.S && cycleCnt(i) + 1.U >= cycles(i)) || stride(i) =/= 0.S
  }.reduce(_&_)

  // read real valid: the last level with stride = 0 and cycleCnt == 0
  val rRealValid = (0 until agNestLevels).map{ i =>
    (stride(i) === 0.S && cycleCnt(i) === 0.U) || stride(i) =/= 0.S
  }.reduce(_&_)

  // in -> sram
  val wValid = isStore && launch && (iiCnt === 0.U)
  val wData = io.in(0) // { if(mode == FIFO_MODE) io.in(0) else io.in(1) }
  // sram -> out
  val raValid = !isStore && launch && (iiCnt === 0.U)// read address valid
  val addr = Wire(UInt(addrWidth.W))
  val genAddrReg = RegInit(0.S((addrWidth+1).W))

  val strideMap = (agNestLevels-1 to 0 by -1).map{ i =>
    if(i == 0) {
      true.B -> stride(i)
    }else{
      cycleCntEnd(i-1) -> stride(i)
//      (0 until i).map(cycleCntEnd(_)).reduce(_&_) -> stride(i)
    }
  }

  val realEn = (wValid && (wRealValid || useAddr)) || (raValid && (rRealValid || useAddr))

  when(!launch){
    genAddrReg := Cat(0.S(1.W), baseAddr).asSInt
  }.elsewhen(iiEnd){
    genAddrReg := genAddrReg + PriorityMux(strideMap)
  }


  if(mode == FIFO_MODE) {
    addr := genAddrReg.asUInt
  } else {
    when(useAddr & isStore){
      addr := (baseAddr + io.in(1)(dataWidth-1, lgDataByte)) // io.in(1) represents address in bytes
    }.elsewhen(useAddr & !isStore){
      addr := (baseAddr + io.in(0)(dataWidth-1, lgDataByte)) // io.in(0) represents address in bytes
    }.otherwise{
      addr := genAddrReg.asUInt
    }
  }

  if(addRegSram > 0){ // add reg, write/read latency + 1/2
    val addrReg = RegInit(0.U(addrWidth.W))
    when(wValid || raValid){
      addrReg := addr
    }
    val wDataReg = RegInit(0.U(dataWidth.W))
    when(wValid){
      wDataReg := wData
    }
    io.sram.en := RegNext(realEn)  // RegNext(wValid || raValid)
    io.sram.we := RegNext(Mux(wValid, ((1 << weWidth) - 1).U, 0.U))
    io.sram.addr := addrReg
    io.sram.din := wDataReg
  }else{ // not add reg, write/read latency is 0/1
    io.sram.en := realEn // wValid || raValid
    io.sram.we := Mux(wValid, ((1 << weWidth) - 1).U, 0.U)
    io.sram.addr := addr
    io.sram.din := wData
  }

  if(addRegSram > 1){
    val dout = RegInit(0.U(dataWidth.W))
    when(RegNext(io.sram.en && (io.sram.we === 0.U))){ // READ Latency should be 1
      dout := io.sram.dout
    }
    io.out(0) := dout
  }else{
    io.out(0) := io.sram.dout
  }


}