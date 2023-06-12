package tram

import chisel3._
import chisel3.util._
import tram.dsa.SRAMIO


/** Configuration Controller, read config data and address from SRAM and output to config bus
 * @param dataWidthSram      SRAM data width in bits
 * @param addrWidthSram      SRAM address width (+1 every data width)
 * @param hasMaskSram        if SRAM has write data byte mask
 * @param addRegSram         SRAM read latency, 0 : 1; 1 : 2; 2 : 3;
 * @param cfgDataWidth       Config data width in bits
 * @param cfgAddrWidth       Config address width
 * @param cfgAddrWidthAlign  Config address width aligned to power2
 * @param cfgRegNum          Config chain register number, the last config data arrive in the last row of IOB after cfgRegNum cycles
 */
class ConfigController(dataWidthSram: Int, addrWidthSram: Int, hasMaskSram: Boolean, addRegSram: Int,
                       cfgDataWidth: Int, cfgAddrWidth: Int, cfgAddrWidthAlign: Int, cfgRegNum: Int) extends Module {
  val io = IO(new Bundle {
    val start = Input(Bool()) // pulse signal
    val done = Output(Bool()) // config done, keep true until next start
//    val busy = Output(Bool())
    val base_addr = Input(UInt(addrWidthSram.W))
    val cfg_num = Input(UInt(cfgAddrWidth.W))
    val sram = Flipped(new SRAMIO(dataWidthSram, addrWidthSram, hasMaskSram))
    // config bus
    val cfg_en   = Output(Bool())
    val cfg_addr = Output(UInt(cfgAddrWidth.W))
    val cfg_data = Output(UInt(cfgDataWidth.W))
  })

  val readLatencySram = addRegSram + 1
  val sramDout = {
    if(addRegSram > 0) RegNext(io.sram.dout)
    else io.sram.dout
  }

  val cfgWidth = cfgDataWidth + cfgAddrWidthAlign
  val s_idle :: s_data :: s_wait :: Nil = Enum(3)
  val state = RegInit(s_idle)
  val cnt = RegInit(0.U(cfgAddrWidth.W))
  val regWidth = (((dataWidthSram max cfgWidth) + dataWidthSram - 1) / dataWidthSram  + readLatencySram + 1) * dataWidthSram
  val dataReg = RegInit(0.U(regWidth.W))
  val wOffset = RegInit(0.U(log2Ceil(regWidth+1).W))   // write offset in dataReg
  val availOffset = RegInit(0.U(log2Ceil(regWidth+1).W)) // available offset in dataReg for new SRAM data
  val sramRen = (availOffset <= (regWidth - dataWidthSram).U) && (state === s_data) // SRAM Read enable
  val wen = ShiftRegister(sramRen, readLatencySram) // sram read data is valid, write to dataReg
  val ren = (wOffset >= cfgWidth.U) && (cnt < io.cfg_num) // read from dataReg, =cfg_en
  val waitCnt = RegInit(0.U(log2Ceil(cfgRegNum+1).W))
  val done = RegInit(false.B)
  io.done := done
//  io.busy := (state === s_data)
  switch(state){
    is(s_idle){
      when(io.start){
        state := s_data
        done := false.B
      }
    }
    is(s_data){
      when(cnt >= io.cfg_num){
        state := s_wait
      }
    }
    is(s_wait){
      when(waitCnt >= cfgRegNum.U){
        state := s_idle
        done := true.B
      }
    }
  }
  // cfg_en count
  when(state === s_idle){
    cnt := 0.U
  }.elsewhen(ren){
    cnt := cnt + 1.U
  }
  // wait count
  when(state === s_idle){
    waitCnt := 0.U
  }.elsewhen(state === s_wait){
    waitCnt := waitCnt + 1.U
  }
  // R/W dataReg
  when(state === s_idle){
    availOffset := 0.U
  }.elsewhen(sramRen && ren){
    availOffset := availOffset + dataWidthSram.U - cfgWidth.U
  }.elsewhen(sramRen){
    availOffset := availOffset + dataWidthSram.U
  }.elsewhen(ren){
    availOffset := availOffset - cfgWidth.U
  }

  when(state === s_idle){
    wOffset := 0.U
    dataReg := 0.U
  }.elsewhen(wen && ren){
    wOffset := wOffset + dataWidthSram.U - cfgWidth.U
    dataReg := ((dataReg << (regWidth.U - wOffset)) >> (regWidth.U + cfgWidth.U - wOffset)).asUInt |
                (sramDout << (wOffset - cfgWidth.U)).asUInt
  }.elsewhen(wen){
    wOffset := wOffset + dataWidthSram.U
    dataReg := ((dataReg << (regWidth.U - wOffset)) >> (regWidth.U - wOffset)).asUInt |
      (sramDout << wOffset).asUInt
  }.elsewhen(ren){
    wOffset := wOffset - cfgWidth.U
    dataReg := (dataReg >> cfgWidth.U).asUInt
  }

  val addrSram = RegInit(0.U(addrWidthSram.W))
  when(state === s_idle){
    addrSram := io.base_addr
  }.elsewhen(sramRen){
    addrSram := addrSram + 1.U
  }
//  io.sram.en := sramRen
  io.sram.we := 0.U
//  io.sram.addr := addrSram
  io.sram.din := 0.U

  if(addRegSram > 1){ // add reg to en, read latency + 3
    io.sram.en := RegNext(sramRen)
    io.sram.addr := RegNext(addrSram)
  }else{ // not add reg to en, read latency is 1/2
    io.sram.en := sramRen
    io.sram.addr := addrSram
  }

  io.cfg_en := ren
  io.cfg_addr := dataReg(cfgWidth-1, cfgDataWidth)
  io.cfg_data := dataReg(cfgDataWidth-1, 0)

}


