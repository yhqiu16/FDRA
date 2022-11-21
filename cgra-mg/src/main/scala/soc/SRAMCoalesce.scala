package tram

import chisel3._
import chisel3.util._
import tram.dsa._


/** Cascade multiple SRAM banks into one bank with larger depth
  * @param width    data width
  * @param lgDepth  log2(single SRAM bank depth)
  * @param hasMask  if has write data byte mask
  * @param nBanks   SRAM bank number
  */
class SRAMBanksCascade(width: Int, lgDepth: Int, hasMask: Boolean, nBanks: Int) extends Module {
  val cascAddrWidth = lgDepth + log2Ceil(nBanks)
  val io = IO(new Bundle {
    val orig = Vec(nBanks, Flipped(new SRAMIO(width, lgDepth, hasMask)))
    val casc = new SRAMIO(width, cascAddrWidth, hasMask)
  })

  for(i <- 0 until nBanks){
    val inSel = io.casc.en && io.casc.addr(cascAddrWidth-1, lgDepth) === i.U
    io.orig(i).en := inSel
    io.orig(i).we := Mux(inSel, io.casc.we, 0.U)
    io.orig(i).addr := io.casc.addr(lgDepth-1, 0)
    io.orig(i).din := io.casc.din
  }
  val table = io.orig.zipWithIndex.map{ case (orig, i) => i.U -> orig.dout }
  io.casc.dout := MuxLookup(io.casc.addr(cascAddrWidth-1, lgDepth), 0.U, table)
}

/** Coalesce multiple SRAM banks and provide the same number of SRAM interfaces capable of accessing all the banks
  * @param width    data width
  * @param lgDepth  log2(single SRAM bank depth)
  * @param hasMask  if has write data byte mask
  * @param nBanks   SRAM bank number
  */
class SRAMBanksCoalesce(width: Int, lgDepth: Int, hasMask: Boolean, nBanks: Int) extends Module {
  val coalAddrWidth = lgDepth + log2Ceil(nBanks)
  val io = IO(new Bundle {
    val orig = Vec(nBanks, Flipped(new SRAMIO(width, lgDepth, hasMask)))
    val coal = Vec(nBanks, new SRAMIO(width, coalAddrWidth, hasMask))
  })

  for(i <- 0 until nBanks){
    val inSel = io.coal.map( c => c.en && c.addr(coalAddrWidth-1, lgDepth) === i.U )
    io.orig(i).en := MuxCase(false.B, inSel.zipWithIndex.map{ case (sel, i) => sel-> io.coal(i).en })
    io.orig(i).we := MuxCase(0.U, inSel.zipWithIndex.map{ case (sel, i) => sel-> io.coal(i).we })
    io.orig(i).addr := MuxCase(0.U, inSel.zipWithIndex.map{ case (sel, i) => sel-> io.coal(i).addr(lgDepth-1, 0) })
    io.orig(i).din := MuxCase(0.U, inSel.zipWithIndex.map{ case (sel, i) => sel-> io.coal(i).din })
    val outSel = (0 until nBanks).map{ j =>
      RegNext(io.coal(i).en && io.coal(i).addr(coalAddrWidth-1, lgDepth) === j.U) }
    io.coal(i).dout := MuxCase(0.U, outSel.zipWithIndex.map{ case (sel, j) => sel-> io.orig(j).dout })
//    io.orig(i).en := PriorityMux(inSel, io.coal.map(_.en))
//    io.orig(i).we := PriorityMux(inSel, io.coal.map(_.we))
//    io.orig(i).addr := PriorityMux(inSel, io.coal.map(_.addr(lgDepth-1, 0)))
//    io.orig(i).din := PriorityMux(inSel, io.coal.map(_.din))
//    val outSel = (0 until nBanks).map{ j =>
//      RegNext(io.coal(i).en && io.coal(i).addr(coalAddrWidth-1, lgDepth) === j.U) }
//    io.coal(i).dout := PriorityMux(outSel, io.orig.map(_.dout))
  }
}


/** Coalesce SRAM banks, divided into n groups where the internal banks are coalesced
  * @param width          data width
  * @param lgDepth        log2(single SRAM bank depth)
  * @param hasMask        if has write data byte mask
  * @param nBanks         Total SRAM bank number
  * @param coalesceBanks  coalesceing bank number
  */
class SRAMCoalesce(width: Int, lgDepth: Int, hasMask: Boolean, nBanks: Int, coalesceBanks: Int) extends Module {
  val coalAddrWidth = lgDepth + log2Ceil(coalesceBanks)
  val io = IO(new Bundle {
    val orig = Vec(nBanks, Flipped(new SRAMIO(width, lgDepth, hasMask)))
    val coal = Vec(nBanks, new SRAMIO(width, coalAddrWidth, hasMask))
  })

  // divide all the banks into n groups where the internal banks are coalesced
  // eg. {0, 1}, {2, 3}, {4, 5},...
  for(i <- 0 until nBanks by coalesceBanks){
    val coalBanks = coalesceBanks min (nBanks-i) // last group may have banks no more than coalesceBanks
    val group = Module(new SRAMBanksCoalesce(width, lgDepth, hasMask, coalBanks))
    for(j <- 0 until coalBanks){
      group.io.orig(j) <> io.orig(i+j)
      group.io.coal(j) <> io.coal(i+j)
    }
  }

}