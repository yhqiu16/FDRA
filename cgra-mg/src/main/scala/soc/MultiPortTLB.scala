package tram

import chisel3._
import chisel3.util._
import freechips.rocketchip.tile._
import freechips.rocketchip.config._
import freechips.rocketchip.diplomacy._
import freechips.rocketchip.rocket._
import freechips.rocketchip.tilelink._
import testchipip.TLHelper

// SFence instruction invalidate TLB entries
// rs1: virtual address to invalidate
// rs2: address space id (asid)
// rs1 and rs2 == x0, invalidate all the TLB entries
//class SFenceReq(implicit p: Parameters) extends CoreBundle()(p) {
//  val rs1 = Bool() // if has rs1
//  val rs2 = Bool() // if has rs2
//  val addr = UInt(width = vaddrBits) // virtual address to invalidate
//  val asid = UInt(width = asIdBits max 1) // TODO zero-width
//  val hv = Bool()  // hypervisor virtual address
//  val hg = Bool()  // hypervisor guest physical
//}
//
//class TLBReq(lgMaxSize: Int)(implicit p: Parameters) extends CoreBundle()(p) {
//  val vaddr = UInt(width = vaddrBitsExtended)
//  val passthrough = Bool()
//  val size = UInt(width = log2Ceil(lgMaxSize + 1))
//  val cmd  = Bits(width = M_SZ)
//  val prv = UInt(PRV.SZ.W)  // privilege mode
//  val v = Bool()            // hypervisor
//}
//
//class TLBExceptions(implicit p: Parameters) extends CoreBundle()(p) {
//  val ld = Bool()
//  val st = Bool()
//  val inst = Bool()
//  val v = Bool()
//}
//
//class TLBResp(implicit p: Parameters) extends CoreBundle()(p) {
//  // lookup responses
//  val miss = Bool()
//  val paddr = UInt(width = paddrBits)
//  val gpa = UInt(vaddrBitsExtended.W)  // guest physical address
//  val gpa_is_pte = Bool()
//  val pf = new TLBExceptions  // page fault (virtual address)
//  val gf = new TLBExceptions  // gpa fault
//  val ae = new TLBExceptions  // access exception, violate PMA, physical memory attributes
//  val ma = new TLBExceptions  // misalignment access
//  val cacheable = Bool()
//  val must_alloc = Bool()
//  val prefetchable = Bool()
//}

class TLBWrapReq(val lgMaxSize: Int)(implicit p: Parameters) extends CoreBundle {
  val vaddr = UInt(vaddrBitsExtended.W)       // virtual address, aligned to size
  val size = UInt(log2Ceil(lgMaxSize + 1).W)  //  2^size
  val cmd  = UInt(M_SZ.W)   // load, store, ...
  val status = new MStatus  // TODO: really need?
}

class TLBWrapResp(implicit p: Parameters) extends CoreBundle {
  val miss = Bool()
  val paddr = UInt(paddrBits.W)
}

class TLBExceptionIO extends Bundle {
  val interrupt = Output(Bool())
  val flush = Input(Bool())  // invalidate all the TLB entries
}

/** TLB Wrapper
  * @param nWays       number of ways in TLB
  * @param lgMaxSize   log2( the max size of the TLB request)
  */
class TLBWrap(nWays: Int, lgMaxSize: Int)(implicit edge: TLEdgeOut, p: Parameters) extends CoreModule {
  val io = new Bundle {
    val req = Flipped(Decoupled(new TLBWrapReq(lgMaxSize)))
    val resp = Decoupled(new TLBWrapResp)
    val ptw = new TLBPTWIO
    val exp = new TLBExceptionIO

  }
  val s_idle :: s_resp :: s_retry :: Nil = Enum(3)
  val state = RegInit(s_idle)
//  val status = Reg(new MStatus)
//  val cmd = Reg(UInt(M_SZ.W))
  val retried = RegInit(false.B)

  val tlb = Module(new TLB(false, lgMaxSize, TLBConfig(nSets=1, nWays=nWays)))

  switch(state){
    is(s_idle){
      when(tlb.io.req.fire){
        state := s_resp
        retried := false.B
//        status := io.req.bits.status
//        cmd := io.req.bits.cmd
      }
    }
    is(s_resp){
      when(tlb.io.req.ready && io.resp.ready && ((!tlb.io.resp.miss) || retried)){
        state := s_idle
      }.elsewhen(tlb.io.req.ready && tlb.io.resp.miss && (!retried)){ // only retry once
        state := s_retry
      }
    }
    is(s_retry){ // TLB miss, retry
      when(tlb.io.req.fire){
        state := s_resp
        retried := true.B
      }
    }
  }

  // keep io.req.bits before the next fire
  val req_reg = Reg(chiselTypeOf(io.req.bits))
  when(tlb.io.req.fire){
    req_reg := io.req.bits
  }
  val req = Mux(tlb.io.req.fire, io.req.bits, req_reg)
  io.req.ready := tlb.io.req.ready && (state === s_idle)
  tlb.io.req.valid := (io.req.valid && (state === s_idle)) || (state === s_retry)
  tlb.io.req.bits.vaddr := req.vaddr
  tlb.io.req.bits.size := req.size
  tlb.io.req.bits.cmd := req.cmd
  tlb.io.req.bits.passthrough := false.B
  tlb.io.req.bits.prv := req.status.prv
  tlb.io.req.bits.v := false.B

  io.ptw <> tlb.io.ptw
  tlb.io.ptw.status := req.status
  tlb.io.kill := false.B
  tlb.io.sfence.valid := io.exp.flush
  tlb.io.sfence.bits.rs1 := false.B
  tlb.io.sfence.bits.rs2 := false.B
  tlb.io.sfence.bits.addr := DontCare
  tlb.io.sfence.bits.asid := DontCare
  tlb.io.sfence.bits.hv := false.B
  tlb.io.sfence.bits.hg := false.B

  io.resp.valid := (state === s_resp) && tlb.io.req.ready && ((!tlb.io.resp.miss) || retried)
  io.resp.bits.miss := tlb.io.resp.miss
  io.resp.bits.paddr := tlb.io.resp.paddr

  val interrupt = RegInit(false.B)
  val exception = Mux(req.cmd === M_XRD, tlb.io.resp.pf.ld || tlb.io.resp.ae.ld || tlb.io.resp.ma.ld,
    tlb.io.resp.pf.st || tlb.io.resp.ae.st || tlb.io.resp.ma.st)
  when ((state === s_resp) && tlb.io.req.ready && exception) {
    interrupt := true.B
  }
  when (interrupt && tlb.io.sfence.fire) {
    interrupt := false.B
  }
  io.exp.interrupt := interrupt

}


class MPTLBIO(val lgMaxSize: Int)(implicit p: Parameters) extends CoreBundle {
//  val lgMaxSize = log2Ceil(coreDataBytes)
  val req = Flipped(Decoupled(new TLBWrapReq(lgMaxSize)))
  val resp = Decoupled(new TLBWrapResp)
}

/** TLB with multiple slave ports
  * @param nSlaves        slave number
  * @param lgMaxSize      log2( the max size of the TLB request)
  * @param nWaysOfTLB     number of ways in TLB
  * @param useSharedTLB   if use a shared TLB among the slaves
  */
class MultiPortTLB(nSlaves: Int, lgMaxSize: Int, nWaysOfTLB: Int, useSharedTLB: Boolean)
                 (implicit edge: TLEdgeOut, p: Parameters) extends CoreModule {
  val nTLBs = if (useSharedTLB) 1 else nSlaves
//  val lgMaxSize = log2Ceil(coreDataBytes)
  val io = IO(new Bundle {
    val slaves = Vec(nSlaves, new MPTLBIO(lgMaxSize))
    val ptw = Vec(nTLBs, new TLBPTWIO)
    val exp = Vec(nTLBs, new TLBExceptionIO)
  })

  val tlbs = Seq.fill(nTLBs)(Module(new TLBWrap(nWaysOfTLB, lgMaxSize)(edge, p)))

  io.ptw <> VecInit(tlbs.map(_.io.ptw))
  io.exp <> VecInit(tlbs.map(_.io.exp))

  val tlbArbOpt = if (useSharedTLB) Some(Module(new RRArbiter(new TLBWrapReq(lgMaxSize), nSlaves))) else None
  val slaveRespReadys = Wire(Vec(nSlaves, Bool()))

  if (useSharedTLB) {
    val tlbArb = tlbArbOpt.get
    val tlb = tlbs.head
    tlb.io.req <> tlbArb.io.out
    tlb.io.resp.ready := slaveRespReadys.reduce(_ || _)
  }else{
    tlbs.zipWithIndex.map{ case (tlb, i) => tlb.io.resp.ready := slaveRespReadys(i)}
  }

  io.slaves.zipWithIndex.foreach { case (slave, i) =>
    val tlb = if (useSharedTLB) tlbs.head else tlbs(i)
    val tlbReq = if (useSharedTLB) tlbArbOpt.get.io.in(i) else tlb.io.req
    val tlbReqFire = if (useSharedTLB) tlbArbOpt.get.io.in(i).fire else tlb.io.req.fire
    tlbReq <> slave.req

    val s_idle :: s_resp :: Nil = Enum(2)
    val state = RegInit(s_idle)
    switch(state) {
      is(s_idle) {
        when(tlbReqFire) {
          state := s_resp
        }
      }
      is(s_resp) {
        when(slave.resp.fire) {
          state := s_idle
        }
      }
    }

    slave.resp.valid := (state === s_resp) && tlb.io.resp.valid
    slave.resp.bits := tlb.io.resp.bits
    slaveRespReadys(i) := (state === s_resp) && slave.resp.ready
  }

}


