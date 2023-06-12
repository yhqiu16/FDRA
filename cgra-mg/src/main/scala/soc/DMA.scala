package tram

import chisel3._
import chisel3.util._
import freechips.rocketchip.tile._
import freechips.rocketchip.config._
import freechips.rocketchip.diplomacy._
import freechips.rocketchip.rocket._
import freechips.rocketchip.tilelink._
import testchipip.TLHelper
import tram.dsa.TrueDualPortSRAM

// DMA read/write request interface
// remote memory : virtual address
class DMAReq(lgMaxLen: Int, idWidth: Int)(implicit p: Parameters) extends CoreBundle{
  val addr = UInt(coreMaxAddrBits.W)  // base address
  val len = UInt(lgMaxLen.W)          // transferring data length in bytes
  val id = UInt(idWidth.W)            // can be zero
  val status = new MStatus
}

// DMA stream interface
class DMAStream(dataWidth: Int, hasMask: Boolean, idWidth: Int) extends Bundle{
  val data = UInt(dataWidth.W) // aligned to byte
  val mask = UInt({if(hasMask) (dataWidth/8) else 0}.W) // byte mask
  val id = UInt(idWidth.W)
  val last = Bool()  // last data flag
}

// Exception interface
class ExceptionIF(idWidth: Int) extends Bundle{
  val req = Output(Bool()) // req keep true until ack is true
  val ack = Input(Bool())
  val id = Output(UInt(idWidth.W))
}

// DMA Stream read interface
class DMAStreamReadIF(lgMaxLen: Int, dataWidth: Int, hasMask: Boolean, idWidth: Int)(implicit p: Parameters)
  extends CoreBundle{
  val req = Flipped(Decoupled(new DMAReq(lgMaxLen, idWidth)))
  val stream = Decoupled(new DMAStream(dataWidth, hasMask, idWidth))
  val exp = new ExceptionIF(idWidth)
}

// DMA Stream write interface
class DMAStreamWriteIF(lgMaxLen: Int, dataWidth: Int, hasMask: Boolean, idWidth: Int)(implicit p: Parameters)
  extends CoreBundle{
  val req = Flipped(Decoupled(new DMAReq(lgMaxLen, idWidth)))
  val stream = Flipped(Decoupled(new DMAStream(dataWidth, hasMask, idWidth)))
  val exp = new ExceptionIF(idWidth)
}


class ReorderBufferReq(idWidth: Int, lgSize: Int) extends Bundle{
  val id = UInt(idWidth.W)
  val size = UInt(lgSize.W)
}

/** Reorder the response data to make its order consistent with the request order
  * @param dataWidth      data width
  * @param hasMask        if has mask signal in the stream interface
  * @param maxLgBurstSize log2(max burst size of a transaction)
  * @param nReqInflight   supported dma requests inflight
  */
class ReorderBuffer(dataWidth: Int, hasMask: Boolean, maxLgBurstSize: Int, nReqInflight: Int) extends Module{
  val idWidth = log2Ceil(nReqInflight)
  val lgSize = log2Ceil(maxLgBurstSize)
  val io = IO(new Bundle{
    val req = Flipped(Decoupled(new ReorderBufferReq(idWidth, lgSize)))
    val streamin = Flipped(Decoupled(new DMAStream(dataWidth, hasMask, idWidth)))
    val streamout = Decoupled(new DMAStream(dataWidth, hasMask, idWidth))
  })
  val bufSize = (1 << maxLgBurstSize) * nReqInflight / (dataWidth/8)
  val bufAddrWidth = log2Ceil(bufSize)
  val offsetWidth = maxLgBurstSize - log2Ceil(dataWidth/8)
  val buffer = Module(new TrueDualPortSRAM(dataWidth, bufAddrWidth, hasMask))
  val statusTable = RegInit(VecInit(Seq.fill(nReqInflight){0.U(2.W)})) // STATUS for each req-id, 0: IDLE; 1: REQ; 2: RES
  val reqQue = Module(new Queue(new ReorderBufferReq(idWidth, lgSize), nReqInflight))

  val ready = RegInit(false.B)
  io.req.ready := ready
  // Cache request in Queue
  when(!ready && io.req.valid && statusTable(io.req.bits.id) === 0.U && reqQue.io.enq.ready){
    ready := true.B
    reqQue.io.enq.valid := true.B
    reqQue.io.enq.bits := io.req.bits
  }.otherwise{
    ready := false.B
    reqQue.io.enq.valid := false.B
    reqQue.io.enq.bits := DontCare
  }

  // if data packet arrive in order, stream out directly
  // otherwise, cache in buffer and reorder
  val s_idle :: s_bypass :: s_read :: Nil = Enum(3)
  val state = RegInit(s_idle)
  buffer.io.b.we := 0.U
  buffer.io.b.din := 0.U
  val rdOffset = RegInit(0.U((offsetWidth+1).W))
  buffer.io.b.addr := Cat(reqQue.io.deq.bits.id, rdOffset(offsetWidth-1, 0))
  val doutValid = RegInit(false.B)
  val doutLast = RegInit(false.B)
  val bypass = RegInit(false.B)
  val buf_we = RegInit(false.B) // write enable
  val MIN_SIZE = log2Ceil(dataWidth/8).U
  val alignSize = Wire(UInt(lgSize.W))
  when(reqQue.io.deq.bits.size <= MIN_SIZE){
    alignSize := 0.U
  }.otherwise{
    alignSize := reqQue.io.deq.bits.size - MIN_SIZE
  }
  val rdDataSize = (1.U << alignSize).asUInt // DATA Number
  switch(state){
    is(s_idle){
      when(reqQue.io.deq.valid && io.streamin.valid && !buf_we && reqQue.io.deq.bits.id === io.streamin.bits.id){
        state := s_bypass
        bypass := true.B
      }.elsewhen(reqQue.io.deq.valid && statusTable(reqQue.io.deq.bits.id) === 2.U){  // buffer data is ready
        doutValid := true.B
        doutLast := (rdOffset + 1.U >= rdDataSize)
        state := s_read
        rdOffset := rdOffset + 1.U
      }
    }
    is(s_bypass){ // bypass streamin to streamout
      when(io.streamout.ready && io.streamin.valid && io.streamin.bits.last){
        state := s_idle
        bypass := false.B
      }
    }
    is(s_read){ // read data from buffer
      when(io.streamout.ready && rdOffset < rdDataSize){
        rdOffset := rdOffset + 1.U
        doutLast := (rdOffset + 1.U >= rdDataSize)
      }.elsewhen(io.streamout.ready){ // LAST data
        rdOffset := 0.U
        doutValid := false.B
        doutLast := false.B
        state := s_idle
      }
    }
  }

  reqQue.io.deq.ready := io.streamout.ready && ((state === s_read && rdOffset >= rdDataSize) ||
        (state === s_bypass && io.streamin.valid && io.streamin.bits.last))

  io.streamin.ready := (bypass && io.streamout.ready) || buf_we

  // Cache data in buffer, data may arrive out of order
  when(!buf_we && state === s_idle && reqQue.io.deq.valid && io.streamin.valid && reqQue.io.deq.bits.id =/= io.streamin.bits.id){
    buf_we := true.B
  }.elsewhen(buf_we && io.streamin.valid && io.streamin.bits.last){
    buf_we := false.B
  }
  buffer.io.a.en := buf_we
  buffer.io.a.we := {if(hasMask) io.streamin.bits.mask else buf_we.asUInt}
  buffer.io.a.din := io.streamin.bits.data
  val wrOffset = RegInit(0.U(offsetWidth.W))
  buffer.io.a.addr := Cat(io.streamin.bits.id, wrOffset)
  when(buf_we && io.streamin.valid && io.streamin.bits.last){
    wrOffset := 0.U
  }.elsewhen(buf_we && io.streamin.valid){
    wrOffset := wrOffset + 1.U
  }

  // read data from buffer
  when(state === s_idle && reqQue.io.deq.valid && statusTable(reqQue.io.deq.bits.id) === 2.U){
    buffer.io.b.en := true.B
  }.elsewhen(state === s_read && io.streamout.ready && rdOffset < rdDataSize){
    buffer.io.b.en := true.B
  }.otherwise{
    buffer.io.b.en := false.B
  }

//  when(rdOffset + 1.U === rdDataSize){
//    doutLast := true.B
//  }.elsewhen(io.streamout.ready && rdOffset >= rdDataSize){
//    doutLast := false.B
//  }
//
//  when(state =/= s_read || (io.streamout.ready && rdOffset >= rdDataSize)){
//    doutLast := false.B
//  }.elsewhen(rdOffset + 1.U === rdDataSize){
//    doutLast := true.B
//  }

  io.streamout.valid := doutValid || (bypass && io.streamin.valid)
  io.streamout.bits.data := Mux(bypass, io.streamin.bits.data, buffer.io.b.dout)
  io.streamout.bits.id := reqQue.io.deq.bits.id
  io.streamout.bits.last := doutValid && doutLast || (bypass && io.streamin.bits.last)
  io.streamout.bits.mask := { if (hasMask) ((1 << (dataWidth/8)) - 1).U else DontCare }

  statusTable.zipWithIndex.foreach{ case (status, i) =>
    when(io.req.fire && io.req.bits.id === i.U){
      status := 1.U
    }.elsewhen(buf_we && io.streamin.valid && io.streamin.bits.last && io.streamin.bits.id === i.U){
      status := 2.U
    }.elsewhen(reqQue.io.deq.fire && reqQue.io.deq.bits.id === i.U){
      status := 0.U
    }
  }
}



/** DMA Stream read controller, read data from remote memory with virtual address
  * @param lgMaxDataLen   log2(the max data length in one DMA request)
  * @param dataWidth      data width
  * @param hasMask        if has mask signal in the stream interface
  * @param idWidth        width of the ID for a DMA request
  * @param nReqInflight   supported dma requests inflight
  * @param maxLgSizeTL    log2(max burst size of a TileLink Edge transaction)
  */
class DMAReadController(lgMaxDataLen: Int, dataWidth: Int, hasMask: Boolean, idWidth: Int, nReqInflight: Int, maxLgSizeTL: Int)
                       (implicit p: Parameters) extends LazyModule {
  val node = TLHelper.makeClientNode(TLMasterParameters.v1(
    name = "dma-read-controller",
    sourceId = IdRange(0, nReqInflight) // [0, n) support n requests inflight
//    requestFifo = true // responses in FIFO order, i.e. in same order the corresponding requests were sent ()
  ))

  lazy val module = new LazyModuleImp(this) with HasCoreParameters {
    val (tl, edge) = node.out(0)
//    val dataWidth = tl.params.dataBits
    val io = IO(new Bundle {
      val read = new DMAStreamReadIF(lgMaxDataLen, dataWidth, hasMask, idWidth)
      val tlb = Flipped(new MPTLBIO(maxLgSizeTL))
    })
    val dataByte = dataWidth / 8
    val vaddr = RegInit(0.U(vaddrBitsExtended.W))
    val paddr = RegInit(0.U(paddrBits.W))
    val id = RegInit(0.U(idWidth.W))
    val leftLen = RegInit(0.U(lgMaxDataLen.W))
    val status = Reg(new MStatus)

    //  require(vaddr%dataByte.U == 0.U)
    //  require(leftLen%dataByte.U == 0.U)

//    val pageSize = 1 << pgIdxBits
//    val vpn = vaddr(vaddrBitsExtended - 1, pgIdxBits) // virtual page number
//    val vpo = vaddr(pgIdxBits - 1, 0) // virtual page offset
//    val ppn = paddr(paddrBits - 1, pgIdxBits) // physical page number
    val maxSizeTL = 1 << maxLgSizeTL
    val vxn = vaddr(vaddrBitsExtended - 1, maxLgSizeTL) // virtual number aligned to max TL transfer size
    val vxo = vaddr(maxLgSizeTL - 1, 0) // offset within max TL transfer size

    // ***** When left data cross 1 page ***** //
    val len_align_bw = RegInit(0.U((maxLgSizeTL+1).W))
    len_align_bw := { // packet len ((1<< maxLgSizeTL) - vxo) aligned to power2 value >= it, backward bound is page bound
      val x = Wire(UInt(maxLgSizeTL.W))
      x := Reverse((~vxo).asUInt)
      val x1 = Wire(UInt(maxLgSizeTL.W)) // eg. vxo = 0x11_1010
      x1 := Reverse(x & (x - 1.U) ^ x) //        x1 = 0x00_0100
      x1 << 1
    }
    // second solution:
    //    x := ~vpo // vpo <= 16 bits
    //    val x1 = Wire(UInt(pgIdxBits.W))
    //    x1 := x | (x >> 1).asUInt
    //    val x2 = Wire(UInt(pgIdxBits.W))
    //    x2 := x1 | (x1 >> 2).asUInt
    //    val x3 = Wire(UInt(pgIdxBits.W))
    //    x3 := x2 | (x2 >> 4).asUInt
    //    val x4 = Wire(UInt(pgIdxBits.W))
    //    x4 := x3 | (x3 >> 8).asUInt
    //    (x4+1.U)(pgIdxBits, 0)

    val lgLen_align_bw = OHToUInt(len_align_bw) // log2(len_align_bw)
    val vxo_align_bw = Wire(UInt(maxLgSizeTL.W)) // aligned to len_align_bw
    vxo_align_bw := (~(len_align_bw - 1.U)).asUInt & vxo

    // ***** When left data within one page  ***** //
    val len_align_fw = RegInit(0.U((maxLgSizeTL+1).W))
    len_align_fw := { // packet len (leftlen) aligned to power2 value >= it, forward bound is 0
      val in = Wire(UInt(maxLgSizeTL.W))
      in := vxo ^ (vxo + leftLen - 1.U)
      val x = Reverse(in)              // eg. vxo = 0x01_1010, leftlen = 0x1_0010
      val x1 = Wire(UInt(maxLgSizeTL.W)) //    in = 0x11_0001
      x1 := Reverse(x & (x - 1.U) ^ x) //      x1 = 0x10_0000
      x1 << 1
    }
    val lgLen_align_fw = OHToUInt(len_align_fw) // log2(len_align_fw)
    val vxo_align_fw = Wire(UInt(maxLgSizeTL.W)) // aligned to len_align_fw
    vxo_align_fw := (~(len_align_fw - 1.U)).asUInt & vxo

    val selBw = RegInit(false.B)
//    val len_align = Mux(selBw, len_align_bw, len_align_fw)
    val lgLen_align = RegInit(0.U(lgLen_align_bw.getWidth.W))
    lgLen_align := Mux(selBw, lgLen_align_bw, lgLen_align_fw)
    val vxo_align = RegInit(0.U(maxLgSizeTL.W))
    vxo_align := Mux(selBw, vxo_align_bw, vxo_align_fw)

    class PacketInfo extends Bundle {
      val min_pos = UInt(maxLgSizeTL.W) // data between [min_pos, max_pos] are valid
      val max_pos = UInt(maxLgSizeTL.W)
      val id = UInt(idWidth.W) // request ID
      val exception = Bool()   //  tlb exception
    }
    val packetInfoQue = Module(new Queue(new PacketInfo, nReqInflight))

    // also used to stall extra requests that exceed the max requests in flight since tl.a.ready will not pull down
    val reorderBuffer = Module(new ReorderBuffer(dataWidth, hasMask, maxLgSizeTL, nReqInflight))

    val s_idle :: s_slice :: s_wait :: s_tlb_req :: s_tlb_resp :: s_dma_req :: s_exp :: Nil = Enum(7)
    val state = RegInit(s_idle)

    io.read.req.ready := (state === s_idle)
    switch(state) {
      is(s_idle) {
        when(io.read.req.fire) {
          state := s_slice
          vaddr := io.read.req.bits.addr
          id := io.read.req.bits.id
          leftLen := io.read.req.bits.len
          status := io.read.req.bits.status
        }
      }
      is(s_slice) { // vaddr, vxn, vxo, leftLen are valid
        state := s_wait
        selBw := Mux(vxo + leftLen > maxSizeTL.U, true.B, false.B) // true: still need at least two packets
      }
      is(s_wait){ // wait for lgLen_align, vxo_align are valid
        state := s_tlb_req
      }
      is(s_tlb_req) { // lgLen_align, vxo_align are valid
        when(io.tlb.req.fire) {
          state := s_tlb_resp
        }
      }
      is(s_tlb_resp) {
        when(io.tlb.resp.fire) {
          when(io.tlb.resp.bits.miss) {
            state := s_exp
          }.otherwise {
            state := s_dma_req
            paddr := io.tlb.resp.bits.paddr
          }
        }
      }
      is(s_dma_req) {
        when(tl.a.fire) {
          state := Mux(selBw, s_slice, s_idle)
          leftLen := Mux(selBw, vxo + leftLen - maxSizeTL.U, 0.U)
          vaddr := Cat(vxn + 1.U, 0.U(maxLgSizeTL.W))
        }
      }
//      is(s_dma_resp) {
//        when(edge.done(tl.d)) {
//          state := Mux(selBw, s_slice, s_idle)
//          leftLen := Mux(selBw, vxo + leftLen - maxSizeTL.U, 0.U)
//          vaddr := Cat(vxn + 1.U, 0.U(maxLgSizeTL.W))
//        }
//      }
      is(s_exp){ // exception
        when(packetInfoQue.io.enq.ready){ // cache exception
          state := s_idle
        }
      }
    }

    // TLB request
    io.tlb.req.valid := (state === s_tlb_req)
    io.tlb.req.bits.vaddr := Cat(vxn, vxo_align)
    io.tlb.req.bits.size := lgLen_align
    io.tlb.req.bits.cmd := M_XRD
    io.tlb.req.bits.status := status
    // TLB response
    io.tlb.resp.ready := (state === s_tlb_resp)

    // DMA request
    val tl_id = RegInit(0.U(log2Ceil(nReqInflight).W))
    // submit request to reorderBuffer first to check if DMA request is allowed
    val s_dma_req_idle :: s_dma_req_wait :: s_dma_req_load :: Nil = Enum(3)
    val dma_req_state = RegInit(s_dma_req_idle)
    val reorder_req_valid = RegInit(false.B)
    val tl_a_valid = RegInit(false.B)
    switch(dma_req_state){
      is(s_dma_req_idle){
        when(state === s_tlb_resp && io.tlb.resp.fire && !io.tlb.resp.bits.miss){
          dma_req_state := s_dma_req_wait
          reorder_req_valid := true.B
        }
      }
      is(s_dma_req_wait){
        when(reorderBuffer.io.req.ready){
          reorder_req_valid := false.B
          when(tl.a.ready){
            dma_req_state := s_dma_req_idle
          }.otherwise{
            dma_req_state := s_dma_req_load
            tl_a_valid := true.B
          }
        }
      }
      is(s_dma_req_load){
        when(tl.a.ready){
          dma_req_state := s_dma_req_idle
          tl_a_valid := false.B
        }
      }
    }

    reorderBuffer.io.req.valid := reorder_req_valid
    reorderBuffer.io.req.bits.id := tl_id
    reorderBuffer.io.req.bits.size := lgLen_align

    tl.a.valid := reorderBuffer.io.req.fire || tl_a_valid
    tl.a.bits := edge.Get(
      fromSource = tl_id,
      toAddress = paddr,
      lgSize = lgLen_align
    )._2

    when(tl.a.fire){
      tl_id := Mux(tl_id < (nReqInflight-1).U, tl_id + 1.U, 0.U )
    }

    // reorder data packets to make it consistent with the request order
    tl.d.ready := reorderBuffer.io.streamin.ready
    reorderBuffer.io.streamin.valid := tl.d.valid
    reorderBuffer.io.streamin.bits.data := tl.d.bits.data
    reorderBuffer.io.streamin.bits.id := tl.d.bits.source
    reorderBuffer.io.streamin.bits.last := edge.last(tl.d)
    reorderBuffer.io.streamin.bits.mask := { if (hasMask) ((1 << dataByte) - 1).U else DontCare }

    // cache packet size
    packetInfoQue.io.enq.valid := (tl.a.fire) || (state === s_exp)
    packetInfoQue.io.enq.bits.id := id
    packetInfoQue.io.enq.bits.min_pos := vxo - vxo_align
    val max_pos_tmp = vxo - vxo_align + leftLen
    packetInfoQue.io.enq.bits.max_pos := Mux(max_pos_tmp >= maxSizeTL.U, (maxSizeTL-1).U, max_pos_tmp)
    packetInfoQue.io.enq.bits.exception := (state === s_exp)

    // DMA Stream
    val s_stream_idle :: s_stream_data :: s_stream_exp :: Nil = Enum(3)
    val streamState = RegInit(s_stream_idle)
    val min_pos = RegInit(0.U(maxLgSizeTL.W))
    val max_pos = RegInit(0.U(maxLgSizeTL.W))
    val cache_id = RegInit(0.U(idWidth.W))
    packetInfoQue.io.deq.ready := (streamState === s_stream_idle)
    switch(streamState){
      is(s_stream_idle){
        when(packetInfoQue.io.deq.valid){
          streamState := Mux(packetInfoQue.io.deq.bits.exception, s_stream_exp, s_stream_data)
          min_pos := packetInfoQue.io.deq.bits.min_pos
          max_pos := packetInfoQue.io.deq.bits.max_pos
          cache_id := packetInfoQue.io.deq.bits.id
        }
      }
      is(s_stream_data){
        when(reorderBuffer.io.streamout.fire && reorderBuffer.io.streamout.bits.last) {
          streamState := s_stream_idle
        }
      }
      is(s_stream_exp){
        when(io.read.exp.ack){
          streamState := s_stream_idle
        }
      }
    }

    // DMA exception
    io.read.exp.req := (streamState === s_stream_exp)
    io.read.exp.id := cache_id
    // DMA Stream
    val addr = RegInit(0.U(maxLgSizeTL.W))
    when(streamState === s_stream_idle) {
      addr := 0.U
    }.elsewhen(streamState === s_stream_data && reorderBuffer.io.streamout.fire) {
      addr := addr + dataByte.U
    }
    // over-fetched data is invalid, not transferred on the stream interface
    val isDataInvalid = (addr < min_pos) || (addr >= max_pos)
    reorderBuffer.io.streamout.ready := (streamState === s_stream_data) && (io.read.stream.ready || isDataInvalid)
    io.read.stream.valid := (streamState === s_stream_data) && reorderBuffer.io.streamout.valid && (!isDataInvalid)
    io.read.stream.bits.data := reorderBuffer.io.streamout.bits.data
    io.read.stream.bits.last := reorderBuffer.io.streamout.bits.last || (streamState === s_stream_data && addr + dataByte.U >= max_pos)
    io.read.stream.bits.id := cache_id
    if (hasMask) io.read.stream.bits.mask := reorderBuffer.io.streamout.bits.mask
  }
}


/** DMA Stream write controller, write data to remote memory with virtual address
  * @param lgMaxDataLen   log2(the max data length in one DMA request)
  * @param dataWidth      data width
  * @param hasMask        if has mask signal in the stream interface
  * @param idWidth        width of the ID for a DMA request
  * @param nReqInflight   supported dma requests inflight
  * @param maxLgSizeTL    log2(max burst size of a TileLink Edge transaction)
  */
class DMAWriteController(lgMaxDataLen: Int, dataWidth: Int, hasMask: Boolean, idWidth: Int, nReqInflight: Int, maxLgSizeTL: Int)
                       (implicit p: Parameters) extends LazyModule {
  val node = TLHelper.makeClientNode(TLMasterParameters.v1(
    name = "dma-write-controller",
    sourceId = IdRange(0, nReqInflight) // [0, n) support n requests inflight
//    requestFifo = true // responses in FIFO order, i.e. in same order the corresponding requests were sent
  ))

  lazy val module = new LazyModuleImp(this) with HasCoreParameters {
    val (tl, edge) = node.out(0)
//    val dataWidth = tl.params.dataBits
    val io = IO(new Bundle {
      val write = new DMAStreamWriteIF(lgMaxDataLen, dataWidth, hasMask, idWidth)
      val tlb = Flipped(new MPTLBIO(maxLgSizeTL))
    })
    val dataByte = dataWidth / 8
    val vaddr = RegInit(0.U(vaddrBitsExtended.W))
    val paddr = RegInit(0.U(paddrBits.W))
    val id = RegInit(0.U(idWidth.W))
    val leftLen = RegInit(0.U(lgMaxDataLen.W))
    val status = Reg(new MStatus)

    //  require(vaddr%dataByte.U == 0.U)
    //  require(leftLen%dataByte.U == 0.U)

    //    val pageSize = 1 << pgIdxBits
    //    val vpn = vaddr(vaddrBitsExtended - 1, pgIdxBits) // virtual page number
    //    val vpo = vaddr(pgIdxBits - 1, 0) // virtual page offset
    val maxSizeTL = 1 << maxLgSizeTL
    val vxn = vaddr(vaddrBitsExtended - 1, maxLgSizeTL) // virtual number aligned to max TL transfer size
    val vxo = vaddr(maxLgSizeTL - 1, 0) // offset within max TL transfer size

    // ***** When left data cross 1 page ***** //
    val len_align_bw = RegInit(0.U((maxLgSizeTL+1).W))
    len_align_bw := { // packet len ((1<< maxLgSizeTL) - vxo) aligned to power2 value >= it, backward bound is page bound
      val x = Wire(UInt(maxLgSizeTL.W))
      x := Reverse((~vxo).asUInt)
      val x1 = Wire(UInt(maxLgSizeTL.W)) // eg. vpo = 0x1111_0001_1010
      x1 := Reverse(x & (x - 1.U) ^ x) //      x1 = 0x0000_1000_0000
      x1 << 1
    }

    val lgLen_align_bw = OHToUInt(len_align_bw) // log2(len_align_bw)
    val vxo_align_bw = Wire(UInt(maxLgSizeTL.W)) // aligned to len_align_bw
    vxo_align_bw := (~(len_align_bw - 1.U)).asUInt & vxo

    // ***** When left data within one page  ***** //
    val len_align_fw = RegInit(0.U((maxLgSizeTL+1).W))
    len_align_fw := { // packet len (leftlen) aligned to power2 value >= it, forward bound is 0
      val in = Wire(UInt(maxLgSizeTL.W))
      in := vxo ^ (vxo + leftLen - 1.U)
      val x = Reverse(in) // eg. vpo = 0x0000_0101_1010, leftlen = 0x1_0010
      val x1 = Wire(UInt(maxLgSizeTL.W)) //      in = 0x0000_0011_0001
      x1 := Reverse(x & (x - 1.U) ^ x) //      x1 = 0x0000_0010_0000
      x1 << 1
    }
    val lgLen_align_fw = OHToUInt(len_align_fw) // log2(len_align_fw)
    val vxo_align_fw = Wire(UInt(maxLgSizeTL.W)) // aligned to len_align_fw
    vxo_align_fw := (~(len_align_fw - 1.U)).asUInt & vxo

    val selBw = RegInit(false.B)
//    val len_align = Mux(selBw, len_align_bw, len_align_fw)
//    val lgLen_align = Mux(selBw, lgLen_align_bw, lgLen_align_fw)
//    val vxo_align = Mux(selBw, vxo_align_bw, vxo_align_fw)
    val lgLen_align = RegInit(0.U(lgLen_align_bw.getWidth.W))
    lgLen_align := Mux(selBw, lgLen_align_bw, lgLen_align_fw)
    val vxo_align = RegInit(0.U(maxLgSizeTL.W))
    vxo_align := Mux(selBw, vxo_align_bw, vxo_align_fw)

    val s_idle :: s_slice :: s_wait :: s_tlb_req :: s_tlb_resp :: s_dma_req :: s_exp :: Nil = Enum(7)
    val state = RegInit(s_idle)
    // DMA reuqest ID
    val tl_id = RegInit(0.U(log2Ceil(nReqInflight).W))
    // // DMA Request/Response Table
    // val table = RegInit(VecInit(Seq.fill(nReqInflight){true.B}))

    io.write.req.ready := (state === s_idle)
    switch(state) {
      is(s_idle) {
        when(io.write.req.fire) {
          state := s_slice
          vaddr := io.write.req.bits.addr
          id := io.write.req.bits.id
          leftLen := io.write.req.bits.len
          status := io.write.req.bits.status
        }
      }
      is(s_slice) { // vaddr, vxn, vxo, leftLen are valid
        state := s_wait
        selBw := Mux(vxo + leftLen > maxSizeTL.U, true.B, false.B) // true: still need at least two packets
      }
      is(s_wait){ // wait for lgLen_align, vxo_align are valid
        // when(table(tl_id)){ // make sure this DMA request can be submit
          state := s_tlb_req
        // }
      }
      is(s_tlb_req) { // len_align, lgLen_align, vxo_align are valid
        when(io.tlb.req.fire) {
          state := s_tlb_resp
        }
      }
      is(s_tlb_resp) {
        when(io.tlb.resp.fire) {
          when(io.tlb.resp.bits.miss) {
            state := s_exp
          }.otherwise {
            state := s_dma_req
            paddr := io.tlb.resp.bits.paddr
          }
        }
      }
      is(s_dma_req) {
        when(edge.done(tl.a)) {
          state := Mux(selBw, s_slice, s_idle)
          leftLen := Mux(selBw, vxo + leftLen - maxSizeTL.U, 0.U)
          vaddr := Cat(vxn + 1.U, 0.U(maxLgSizeTL.W))
        }
      }
//      is(s_dma_resp) {
//        when(tl.d.fire) {
//          state := Mux(selBw, s_slice, s_idle)
//          leftLen := Mux(selBw, vxo + leftLen - maxSizeTL.U, 0.U)
//          vaddr := Cat(vxn + 1.U, 0.U(maxLgSizeTL.W))
//        }
//      }
      is(s_exp){ // exception
        when(io.write.exp.ack){ // cache exception
          state := s_idle
        }
      }
    }

    // TLB request
    io.tlb.req.valid := (state === s_tlb_req)
    io.tlb.req.bits.vaddr := Cat(vxn, vxo_align)
    io.tlb.req.bits.size := lgLen_align
    io.tlb.req.bits.cmd := M_XWR
    io.tlb.req.bits.status := status
    // TLB response
    io.tlb.resp.ready := (state === s_tlb_resp)
    // DMA Exception
    io.write.exp.req := (state === s_exp)
    io.write.exp.id := id
    // DMA request
    val addr = RegInit(0.U(maxLgSizeTL.W))
    when(state === s_tlb_resp) {
      addr := vxo_align
    }.elsewhen(state === s_dma_req && tl.a.fire) {
      addr := addr + dataByte.U
    }
    // over-fetched data is invalid, not transferred on the stream interface
    val isDataInvalid = (addr < vxo) || (addr >= vxo + leftLen)
    io.write.stream.ready := (state === s_dma_req) && tl.a.ready && (!isDataInvalid)

    when(edge.done(tl.a)){
      tl_id := Mux(tl_id < (nReqInflight-1).U, tl_id + 1.U, 0.U )
    }
    val mask = RegInit(0.U(dataByte.W))
    mask := Mux(lgLen_align >= log2Ceil(dataByte).U, ((1 << dataByte) - 1).U,
                (1.U << (1.U << lgLen_align).asUInt).asUInt - 1.U)
    tl.a.valid := (state === s_dma_req) && (io.write.stream.valid || isDataInvalid)
    tl.a.bits := edge.Put(
      fromSource = tl_id,
      toAddress = paddr,
      lgSize = lgLen_align,
      data = io.write.stream.bits.data,
      mask = Mux(isDataInvalid, 0.U, mask)
    )._2
    //DMA response
    tl.d.ready := true.B
//    tl.d.ready := (state === s_dma_resp)

    // // DMA Request/Response Table
    // table.zipWithIndex.foreach{ case (t, i) =>
    //   when(edge.done(tl.a) && tl_id === i.U){ // request
    //     t := false.B // unavailable, wait for response
    //   }.elsewhen(tl.d.fire && tl.d.bits.source === i.U){
    //     t := true.B // available
    //   }
    // }
  }
}


/** DMA controller, read/write data from/to remote memory with virtual address
  * @param lgMaxDataLen   log2(the max data length in one DMA request)
  * @param dataWidth      data width
  * @param hasMask        if has mask signal in the stream interface
  * @param idWidth        width of the ID for a DMA request
  * @param nReqInflight   supported dma requests inflight
  * @param maxLgSizeTL    log2(max burst size of a TileLink Edge transaction)
  * @param nWaysOfTLB     number of ways in TLB
  * @param useSharedTLB   if use a shared TLB among the read and write channels
  */
class DMAController(lgMaxDataLen: Int, dataWidth: Int, hasMask: Boolean, idWidth: Int, nReqInflight: Int, maxLgSizeTL: Int, nWaysOfTLB: Int, useSharedTLB: Boolean)
                   (implicit p: Parameters) extends LazyModule {
  val id_node = TLIdentityNode()

  val dma_reader = LazyModule(new DMAReadController(lgMaxDataLen, dataWidth, hasMask, idWidth, nReqInflight, maxLgSizeTL)(p))
  val dma_writer = LazyModule(new DMAWriteController(lgMaxDataLen, dataWidth, hasMask, idWidth, nReqInflight, maxLgSizeTL)(p))

//  val xbar_node = TLXbar(TLArbiter.lowestIndexFirst)
//  xbar_node := TLBuffer(8) := dma_reader.node
//  xbar_node := TLBuffer(8) := dma_writer.node
//  id_node := TLFIFOFixer() := xbar_node

  id_node := dma_reader.node
  id_node := dma_writer.node

  lazy val module = new LazyModuleImp(this) with HasCoreParameters {
//    val dataWidth = id_node.out.head._1.params.dataBits
    val nTLBs = if (useSharedTLB) 1 else 2
    val io = IO(new Bundle {
      val read = new DMAStreamReadIF(lgMaxDataLen, dataWidth, hasMask, idWidth)
      val write = new DMAStreamWriteIF(lgMaxDataLen, dataWidth, hasMask, idWidth)
      val ptw = Vec(nTLBs, new TLBPTWIO)
      val exp = Vec(nTLBs, new TLBExceptionIO)
    })

    implicit val edge = id_node.edges.out.head
    val tlb = Module(new MultiPortTLB(2, maxLgSizeTL, nWaysOfTLB, useSharedTLB)(edge, p))

    io.read <> dma_reader.module.io.read
    io.write <> dma_writer.module.io.write
    dma_reader.module.io.tlb <> tlb.io.slaves(0)
    dma_writer.module.io.tlb <> tlb.io.slaves(1)
    io.ptw <> tlb.io.ptw
    io.exp <> tlb.io.exp
  }
}