package tram

import chisel3._
import chisel3.util._
import firrtl.Utils.True
import freechips.rocketchip.tile._
import freechips.rocketchip.config._
import freechips.rocketchip.diplomacy._
import tram.spec._

import java.io.File

object TramParam {
  val dumpSpec : Boolean = false
  val loadSpec : Boolean = true
  val dumpOperationSet : Boolean = true
  val dumpADG : Boolean = true
  val rootDirPath = (new File("")).getAbsolutePath()
  val tram_spec_filename = rootDirPath + "/generators/tram/cgra-mg/src/main/resources/tram_spec.json"
  val operation_set_filename = rootDirPath + "/generators/tram/cgra-mg/src/main/resources/operations.json"
  val cgra_adg_filename = rootDirPath + "/generators/tram/cgra-mg/src/main/resources/cgra_adg.json"
}

class Tram(opcodes: OpcodeSet)(implicit p: Parameters) extends LazyRoCC(opcodes, nPTWPorts = 1) {
  import TramParam._
  if(dumpSpec){ TramSpec.dumpSpec(tram_spec_filename) }
  if(loadSpec){ TramSpec.loadSpec(tram_spec_filename) }
  TramSpec.attrs("dumpOperationSet") = dumpOperationSet
  if(dumpOperationSet){ TramSpec.attrs("operation_set_filename") = operation_set_filename }
  TramSpec.attrs("dumpOperationSet") = dumpADG
  if(dumpADG){ TramSpec.attrs("cgra_adg_filename") = cgra_adg_filename }
  // scratchpad banks used for IOB
  val lgSizeSpadBank = TramSpec.attrs("spad_bank_lg_size").asInstanceOf[Int]
  val nSpadBanks = TramSpec.attrs("spad_num_banks").asInstanceOf[Int]
  // scratchpad block used for Config
  val lgSizeSpadCfg = TramSpec.attrs("spad_cfg_lg_size").asInstanceOf[Int]
  val cfgSpadBanks = {
    if(lgSizeSpadCfg <= lgSizeSpadBank) 1
    else 1 << (lgSizeSpadCfg - lgSizeSpadBank)
  }
  val spadAddrWidth = lgSizeSpadBank + log2Ceil(nSpadBanks+cfgSpadBanks) // in bytes
  val lgMaxDataLen = spadAddrWidth
  val spadAddrNum = TramSpec.attrs("spad_addr_num").asInstanceOf[Int]
  val spadDataWidth = TramSpec.attrs("spad_data_width").asInstanceOf[Int]
  val cgraDataWidth = TramSpec.attrs("cgra_data_width").asInstanceOf[Int]
  val hasMask = false // spadDataWidth != cgraDataWidth
  val idWidth = TramSpec.attrs("id_width").asInstanceOf[Int]
  val nReqInflight = TramSpec.attrs("dma_num_req_in_flight").asInstanceOf[Int]
  val maxLgSizeTL = TramSpec.attrs("dma_lg_max_burst_size").asInstanceOf[Int]
  val nWaysOfTLB = TramSpec.attrs("tlb_num_ways").asInstanceOf[Int]
  val useSharedTLB = TramSpec.attrs("tlb_is_shared").asInstanceOf[Boolean]
//  val cmdQueDepth = TramSpec.attrs("rs_cmd_queue_depth").asInstanceOf[Int]
  val loadQueDepth = TramSpec.attrs("rs_load_queue_depth").asInstanceOf[Int]
  val storeQueDepth = TramSpec.attrs("rs_store_queue_depth").asInstanceOf[Int]
  val exeQueDepth = TramSpec.attrs("rs_exe_queue_depth").asInstanceOf[Int]
  val streamQueDepth = TramSpec.attrs("ls_stream_queue_depth").asInstanceOf[Int]
  
  override lazy val module = new TramModuleImp(this)
  val dma_node = LazyModule(new DMAController(lgMaxDataLen, spadDataWidth, hasMask, idWidth, nReqInflight, maxLgSizeTL, nWaysOfTLB, useSharedTLB))
  override val tlNode = dma_node.id_node
//  tlNode := dma_node.id_node
}



class TramModuleImp(outer: Tram)(implicit p: Parameters) extends LazyRoCCModuleImp(outer)
  with HasCoreParameters {
  import outer._

  val reservation = Module(new ReservationStation(loadQueDepth, storeQueDepth, exeQueDepth, idWidth))
  val loader = Module(new LoadController(spadAddrWidth, spadAddrNum, lgMaxDataLen, spadDataWidth, hasMask, idWidth, streamQueDepth, loadQueDepth))
  val storer = Module(new StoreController(spadAddrWidth, lgMaxDataLen, spadDataWidth, hasMask, idWidth, streamQueDepth))
  val spad = Module(new Scratchpad(spadAddrWidth, spadAddrNum, lgMaxDataLen, spadDataWidth, hasMask, idWidth, nSpadBanks, lgSizeSpadBank, lgSizeSpadCfg, cgraDataWidth))
  val cgra = Module(new CGRAController(TramSpec.attrs))
  val dmaCtrl = outer.dma_node.module

  io.busy := reservation.io.busy
  io.resp <> reservation.io.resp
  reservation.io.cmd <> io.cmd
  reservation.io.ld <> loader.io.core
  reservation.io.st <> storer.io.core
  reservation.io.ex <> cgra.io.core
  loader.io.dma <> dmaCtrl.io.read
  loader.io.spad <> spad.io.write
  storer.io.dma <> dmaCtrl.io.write
  storer.io.spad <> spad.io.read
  cgra.io.srams_iob <> spad.io.srams
  cgra.io.sram_cfg <> spad.io.sram_last
  io.ptw <> dmaCtrl.io.ptw
  dmaCtrl.io.exp.foreach(_.flush := reservation.io.flush)
  io.interrupt := dmaCtrl.io.exp.map(_.interrupt).reduce(_ || _)
  io.mem.req.valid := false.B

}

