package tram

import chisel3._
import chisel3.util._
import freechips.rocketchip.tile._
import freechips.rocketchip.config._
import tram.dsa._

//// SRAM I/O ports
//class SRAMIO(dataWidth: Int, addrWidth: Int, hasMask: Boolean) extends Bundle {
//  val en   = Input(Bool())
//  val we   = Input(UInt({if(hasMask) (dataWidth/8) else 1}.W))
//  val addr = Input(UInt(addrWidth.W))
//  val din  = Input(UInt(dataWidth.W))
//  val dout = Output(UInt(dataWidth.W))
//}
//
///** True dual port SRAM
//  * @param width    data width
//  * @param lgDepth  log2(SRAM depth)
//  * @param hasMask  if has write data byte mask
//  */
//class TrueDualPortSRAM(width: Int, lgDepth: Int, hasMask: Boolean) extends Module {
//  val io = IO(new Bundle{
//    val a = new SRAMIO(width, lgDepth, hasMask)
//    val b = new SRAMIO(width, lgDepth, hasMask)
//  })
//  val ios = Array(io.a, io.b)
//  if(hasMask){
//    val maskWidth = width/8
//    val mem = SyncReadMem(1<<lgDepth, Vec(maskWidth, UInt(8.W)))
//    ios.map{ x =>
//      x.dout := DontCare
//      when(x.en) {
//        val rwPort = mem(x.addr)
//        when(x.we =/= 0.U) {
//          (0 until maskWidth).map { i =>
//            when(x.we(i).asBool) {
//              rwPort(i) := x.din(i * 8 + 7, i * 8)
//            }
//          }
//        }.otherwise {
//          x.dout := Cat(rwPort.toSeq.reverse)
//        }
//      }
//    }
//  } else {
//    val mem = SyncReadMem(1<<lgDepth, UInt(width.W))
//    ios.map{ x =>
//      x.dout := DontCare
//      when(x.en) {
//        val rwPort = mem(x.addr)
//        when(x.we.asBool) {
//          rwPort := x.din
//        }.otherwise {
//          x.dout := rwPort
//        }
//      }
//    }
//  }
//}


// Scratchpad stream request with physical address
// include multiple addresses for broadcast
class SpadStreamReq(spadAddrWidth: Int, spadAddrNum: Int, lgMaxLen: Int, idWidth: Int)(implicit p: Parameters) extends CoreBundle{
  val addrs = Vec(spadAddrNum, UInt(spadAddrWidth.W))   // start addresses
  val num = UInt(log2Ceil(spadAddrNum).W) // used address number - 1
  val len = UInt(lgMaxLen.W)         // transferring data length in bytes
  val id = UInt(idWidth.W)           // can be zero
}


// Scratchpad Stream read interface
class SpadStreamReadIF(spadAddrWidth: Int, lgMaxLen: Int, dataWidth: Int, hasMask: Boolean, idWidth: Int)
                      (implicit p: Parameters) extends CoreBundle{
  val req = Flipped(Decoupled(new SpadStreamReq(spadAddrWidth, 1, lgMaxLen, idWidth)))
  val stream = Decoupled(new DMAStream(dataWidth, hasMask, idWidth))
  val exp = Flipped(new ExceptionIF(idWidth))
}

// Scratchpad Stream write interface
class SpadStreamWriteIF(spadAddrWidth: Int, spadAddrNum: Int, lgMaxLen: Int, dataWidth: Int, hasMask: Boolean, idWidth: Int)
                       (implicit p: Parameters) extends CoreBundle{
  val req = Flipped(Decoupled(new SpadStreamReq(spadAddrWidth, spadAddrNum, lgMaxLen, idWidth)))
  val stream = Flipped(Decoupled(new DMAStream(dataWidth, hasMask, idWidth)))
  val exp = Flipped(new ExceptionIF(idWidth))
}


/** Multi-bank scratchpad composed of multiple true-dual-port SRAMs
  * @param spadAddrWidth  scratchpad address width, <= 32
  * @param spadAddrNum    scratchpad address number, be multiple to support broadcast write
  * @param lgMaxDataLen   log2(the max data length in one DMA request)
  * @param dataWidth      data width in bits
  * @param hasMask        if has mask signal in the stream interface
  * @param idWidth        width of the ID for a DMA request
  * @param nBanks         scratchpad bank number except the last bank
  * @param lgSizeBank     log2(single bank size in byte), used for IOB
  * @param lgSizeLastBlock log2(last block size in byte), used for config
  * @param dataWidthSram  data width of the srams port
  */
class Scratchpad(spadAddrWidth: Int, spadAddrNum: Int, lgMaxDataLen: Int, dataWidth: Int, hasMask: Boolean, idWidth: Int, nBanks: Int,
                 lgSizeBank: Int, lgSizeLastBlock: Int, dataWidthSram: Int)
                (implicit p: Parameters) extends CoreModule {
  val dataByte = dataWidth/8
  val lgDataByte = log2Ceil(dataByte)
  assert(isPow2(dataByte), "Data width in bytes should be power 2 values.")
  assert(spadAddrWidth >= lgMaxDataLen, "spadAddrWidth should be larger than lgMaxDataLen.")
  val lgDepthSram = lgSizeBank - log2Ceil(dataWidthSram/8)
  val lgDepth = lgSizeBank - lgDataByte
  val lgDepthLast = lgSizeLastBlock - lgDataByte

  val io = IO(new Bundle {
    // stream interfaces
    val read = new SpadStreamReadIF(spadAddrWidth, lgMaxDataLen, dataWidth, hasMask, idWidth)
    val write = new SpadStreamWriteIF(spadAddrWidth, spadAddrNum, lgMaxDataLen, dataWidth, hasMask, idWidth)
    // sram bank interfaces
    val srams = Vec(nBanks, new SRAMIO(dataWidthSram, lgDepthSram, hasMask))
    // last sram block interface
    val sram_last = new SRAMIO(dataWidth, lgDepthLast, hasMask)
  })

  val spad_banks_io = Seq.fill(nBanks){Module(new TrueDualPortSRAMAsym(dataWidth, lgDepth, dataWidthSram, hasMask)).io}
  val spad_last_bank_io = Module(new TrueDualPortSRAM(dataWidth, lgDepthLast, hasMask)).io

  io.srams.zipWithIndex.foreach{ case (sram, i) =>
    sram <> spad_banks_io(i).b
  }
  io.sram_last <> spad_last_bank_io.b

  val s_write_idle :: s_write_data :: Nil = Enum(2)
  val writeState = RegInit(s_write_idle)
//  val writeAddr = RegInit(0.U(spadAddrWidth.W))
//  val writeBankIdx = writeAddr(spadAddrWidth-1, lgSizeBank)
  val writeAddrs = RegInit(VecInit(Seq.fill(spadAddrNum){0.U(spadAddrWidth.W)}))
  val writeBankIdxs = writeAddrs.map{ addr => addr(spadAddrWidth-1, lgSizeBank)}
  val writeMaxAddr = RegInit(0.U(spadAddrWidth.W))
  val writeId = RegInit(0.U(idWidth.W))
  val writeAddrNum = RegInit(0.U(log2Ceil(spadAddrNum).W))
  io.write.req.ready := (writeState === s_write_idle)
  switch(writeState){
    is(s_write_idle){
      when(io.write.req.fire){
        writeState := s_write_data
        writeAddrs := io.write.req.bits.addrs
        writeAddrNum := io.write.req.bits.num
        writeMaxAddr := io.write.req.bits.addrs(0) + io.write.req.bits.len - dataByte.U
        writeId := io.write.req.bits.id
      }
    }
    is(s_write_data){
      when(io.write.exp.req){
        writeState := s_write_idle
      }.elsewhen(io.write.stream.fire){
        (0 until spadAddrNum).foreach{ i =>
          writeAddrs(i) := writeAddrs(i) + dataByte.U
        }
        writeState := Mux(writeAddrs(0) >= writeMaxAddr, s_write_idle, s_write_data)
      }
    }
  }
  val write_exp_ack = RegInit(false.B)
  when(writeState === s_write_data && io.write.exp.req){
    write_exp_ack := true.B
  }.elsewhen(writeState === s_write_idle){
    write_exp_ack := false.B
  }
  io.write.exp.ack := write_exp_ack

  val s_read_idle :: s_read_data :: s_read_data_last :: Nil = Enum(3)
  val readState = RegInit(s_read_idle)
  val readAddr = RegInit(0.U(spadAddrWidth.W))
  val readMaxAddr = RegInit(0.U(spadAddrWidth.W))
  val readId = RegInit(0.U(idWidth.W))
  val readBankIdx = readAddr(spadAddrWidth-1, lgSizeBank)
  val rdataValid = RegInit(false.B)
  val readBankIdxSyn = RegInit(0.U((spadAddrWidth-lgSizeBank).W))
  io.read.req.ready := (readState === s_read_idle)
  switch(readState){
    is(s_read_idle){
      rdataValid := false.B
      when(io.read.req.fire){
        readState := s_read_data
        readAddr := io.read.req.bits.addrs(0)
        readMaxAddr := io.read.req.bits.addrs(0) + io.read.req.bits.len - dataByte.U
        readId := io.read.req.bits.id
      }
    }
    is(s_read_data){
      when(io.read.exp.req){
        readState := s_read_idle
      }.elsewhen(io.read.stream.ready){
        rdataValid := true.B
        readBankIdxSyn := readBankIdx
        readAddr := readAddr + dataByte.U
        readState := Mux(readAddr >= readMaxAddr, s_read_data_last, s_read_data)
      }
    }
    is(s_read_data_last){ // last data
      when(io.read.stream.ready){
        readState := s_read_idle
        rdataValid := false.B
      }
    }
  }
  val read_exp_ack = RegInit(false.B)
  when(readState === s_read_data && io.read.exp.req){
    read_exp_ack := true.B
  }.elsewhen(readState === s_read_idle){
    read_exp_ack := false.B
  }
  io.read.exp.ack := read_exp_ack

  // stream read and write cannot access the same bank simultaneously
  // the requester should make sure no access conflicts
  // access to spad banks
  spad_banks_io.zipWithIndex.foreach{ case (sp, i) =>
//    val en = (0 until spadAddrNum).map{ j =>
//      (writeAddrNum >= j.U && writeBankIdxs(j) === i.U)
//    }.reduce(_|_)
    val sel = (0 until spadAddrNum).map{ j => (writeAddrNum >= j.U && writeBankIdxs(j) === i.U) }
    val en = sel.reduce(_|_)
    when(writeState === s_write_data && en){
//      val addrMap = (0 until spadAddrNum).map{ j =>
//        (writeAddrNum >= j.U && writeBankIdxs(j) === i.U) -> writeAddrs(j)(lgSizeBank-1, lgDataByte)
//      }
      val addrMap = sel.zipWithIndex.map{ case (v, j) => v -> writeAddrs(j)(lgSizeBank-1, lgDataByte) }
      sp.a.en := io.write.stream.fire
      sp.a.we := { if(hasMask) io.write.stream.bits.mask else 1.U }
      sp.a.addr := PriorityMux(addrMap)
      sp.a.din := io.write.stream.bits.data
    }.elsewhen(readState === s_read_data && readBankIdx === i.U){
      sp.a.en := io.read.stream.ready
      sp.a.we := 0.U
      sp.a.addr := readAddr(lgSizeBank-1, lgDataByte)
      sp.a.din := DontCare
    }.otherwise{
      sp.a.en := false.B
      sp.a.we := 0.U
      sp.a.addr := DontCare
      sp.a.din := DontCare
    }
  }
  // access to last spad block, can only have one address
  when(writeState === s_write_data && writeBankIdxs(0) >= nBanks.U){
    spad_last_bank_io.a.en := io.write.stream.fire
    spad_last_bank_io.a.we := { if(hasMask) io.write.stream.bits.mask else 1.U }
    spad_last_bank_io.a.addr := writeAddrs(0)(lgSizeBank-1, lgDataByte)
    spad_last_bank_io.a.din := io.write.stream.bits.data
  }.elsewhen(readState === s_read_data && readBankIdx >= nBanks.U){
    spad_last_bank_io.a.en := io.read.stream.ready
    spad_last_bank_io.a.we := 0.U
    spad_last_bank_io.a.addr := readAddr(lgSizeBank-1, lgDataByte)
    spad_last_bank_io.a.din := DontCare
  }.otherwise{
    spad_last_bank_io.a.en := false.B
    spad_last_bank_io.a.we := 0.U
    spad_last_bank_io.a.addr := DontCare
    spad_last_bank_io.a.din := DontCare
  }

  io.write.stream.ready := (writeState === s_write_data)

  val lastBanks = {
    if(lgSizeLastBlock <= lgSizeBank) 1
    else 1 << (lgSizeLastBlock - lgSizeBank)
  }
  val doutTable1 = spad_banks_io.zipWithIndex.map{ case (sp, i) => (i.U -> sp.a.dout) }
  val doutTable2 = (0 until lastBanks).map{ i => ((i+nBanks).U -> spad_last_bank_io.a.dout) }
  val doutTable = doutTable1 ++ doutTable2
  io.read.stream.valid := rdataValid
  io.read.stream.bits.data := MuxLookup(readBankIdxSyn, 0.U, doutTable)
  io.read.stream.bits.id := readId
  if(hasMask) io.read.stream.bits.mask := ((1 << dataByte)-1).U
}