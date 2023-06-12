package tram.dsa

import chisel3._
import chisel3.util._
import scala.collection.mutable
import scala.collection.mutable.ListBuffer
import tram.ir._
import tram.common.MacroVar._


/** IO Block
 * 
 * @param attrs     module attributes
 */ 
class IOB(attrs: mutable.Map[String, Any]) extends Module with IR {
//  apply(attrs)
  val width = attrs("data_width").asInstanceOf[Int]
  val addrWidthSram = attrs("addr_width_sram").asInstanceOf[Int]  // address width (+1 every data width)
  val hasMaskSram = attrs("has_mask_sram").asInstanceOf[Boolean]  // if has write data byte mask
  val addRegSram = attrs("add_reg_sram").asInstanceOf[Int]        // write/read latency, 0 : 0/1; 1 : 1/2; 2 : 1/3; for both FIFO and SRAM modes
  val mode = attrs("iob_mode").asInstanceOf[Int]                  // 0: FIFO mode (with din, dout), 1: SRAM mode (with addr, din, dout)
  val lgMaxLat = attrs("lg_max_lat").asInstanceOf[Int]            // log2(max in/out latency)
  val lgMaxII = attrs("lg_max_ii").asInstanceOf[Int]              // log2(max in/out Initialization Interval)
  val lgMaxStride = attrs("lg_max_stride").asInstanceOf[Int]      // log2(max address stride, n represent n*dataWidth bits)
  val lgMaxCycles = attrs("lg_max_cycles").asInstanceOf[Int]      // log2(max in/out cycles)
  val agNestLevels = attrs("ag_nest_levels").asInstanceOf[Int]    // nested levels of the address generation
  // cfgParams
  val cfgDataWidth = attrs("cfg_data_width").asInstanceOf[Int]
  val cfgAddrWidth = attrs("cfg_addr_width").asInstanceOf[Int]
  val cfgBlkIndex  = attrs("cfg_blk_index").asInstanceOf[Int]     // configuration index of this block, cfg_addr[width-1 : offset]
  val cfgBlkOffset = attrs("cfg_blk_offset").asInstanceOf[Int]    // configuration offset bit of blocks
  // IOB index
  val iobIndex = attrs("iob_index").asInstanceOf[Int]
  // I/O
  val numInList = attrs("num_input_per_operand").asInstanceOf[ListBuffer[Int]] // the internal in port may connect to multiple external inputs
  val numIn = numInList.sum
  val numOut = 1
  val numInCtrl = { if(mode == FIFO_MODE) 1 else 2 }
  // max delay cycles of the RDU
  val maxDelay = attrs("max_delay").asInstanceOf[Int]
  apply("data_width", width)
  apply("num_input", numIn)
  apply("num_output", numOut)
  apply("num_operands", numInCtrl)
  apply("cfg_blk_index", cfgBlkIndex)
  apply("iob_index", iobIndex)
  apply("iob_mode", mode)
  apply("ag_nest_levels", agNestLevels)
  if(mode != FIFO_MODE){ apply("max_delay", maxDelay) }


  val io = IO(new Bundle {
    val cfg_en   = Input(Bool())
    val cfg_addr = Input(UInt(cfgAddrWidth.W))
    val cfg_data = Input(UInt(cfgDataWidth.W))
    val start = Input(Bool()) // pulse signal, should be valid before latency 0, namely -1
    val done = Output(Bool()) // transfer done, keep true until next start
//    val ii = Input(UInt(lgMaxII.W)) // Initialization Interval, shared among all IOB, so not put in private config
//    val cycles = Input(UInt(lgMaxCycles.W)) // valid in/out cycles, shared among all IOB, so not put in private config
    val en = Input(Bool())
    val in = Input(Vec(numIn, UInt(width.W)))
    val out = Output(Vec(numOut, UInt(width.W)))
    val sram = Flipped(new SRAMIO(width, addrWidthSram, hasMaskSram))
  })

  val numRDUs = { if(mode == FIFO_MODE) 0 else 2 } // FIFO mode, no RDU
  val ioCtrl = Module(new IOController(width, addrWidthSram, hasMaskSram, mode, lgMaxII, lgMaxLat, lgMaxStride, lgMaxCycles, agNestLevels, addRegSram))
  val rdu: SharedRDU = {if(numRDUs > 0) Module(new SharedRDU(width, maxDelay, numRDUs)) else null}
//  val rdus = Array.fill(numRDUs){ Module(new RDU(width, maxDelay)).io }
  val imuxs = numInList.map{ num => Module(new Muxn(width, num)).io } // input MUXs


  // ======= sub_module attribute ========//
  // 1 : IOController
  // 2-n : sub-modules
  val sm_id: mutable.Map[String, Int] = mutable.Map(
    "IOController" -> 1,
    "Muxn" -> 2
  )

  // ======= sub_module instance attribute ========//
  // 0 : this module
  // 1-n : sub-modules
  val smi_id: mutable.Map[String, List[Int]] = mutable.Map(
    "This" -> List(0),
    "IOController" -> List(1),
  )
  var next_smi_id : Int = 2
  if(numRDUs > 0) {
    sm_id += ("RDU" -> 3)
//    smi_id += ("RDU" -> (numInCtrl+2 until numRDUs+numInCtrl+2).toList)
    smi_id += ("RDU" -> List(next_smi_id))
    next_smi_id += 1
  }
  smi_id += "Muxn" -> (next_smi_id until numInCtrl+next_smi_id).toList
  next_smi_id += numInCtrl

  val sub_modules = sm_id.map{case (name, id) => Map(
    "id" -> id,
    "type" -> name
  )}
  apply("sub_modules", sub_modules)

  val instances = smi_id.map{case (name, ids) =>
    ids.map{id => Map(
      "id" -> id,
      "type" -> name,
      "module_id" -> {if(name == "This") 0 else sm_id(name)}
    )}
  }.flatten
  apply("instances", instances)

  // ======= connections attribute ========//
  // apply("connection_format", ("src_id", "src_type", "src_out_idx", "dst_id", "dst_type", "dst_in_idx"))
  // This:src_out_idx is the input index
  // This:dst_in_idx is the output index
  val connections = ListBuffer[(Int, String, Int, Int, String, Int)]()

  ioCtrl.io.sram <> io.sram
  ioCtrl.io.start := io.start
  io.done := ioCtrl.io.done
  io.out(0) := ioCtrl.io.out(0)
  connections.append((smi_id("IOController")(0), "IOController", 0, smi_id("This")(0), "This", 0))

  var offset = 0
  numInList.zipWithIndex.foreach { case (num, i) =>
    imuxs(i).in.zipWithIndex.foreach { case (in, j) =>
      in := io.in(offset+j)
      connections.append((smi_id("This")(0), "This", offset+j, smi_id("Muxn")(i), "Muxn", j))
    }
    if(numRDUs > 0){
      rdu.io.en := io.en
      rdu.io.in(i) := imuxs(i).out
      ioCtrl.io.in(i) := rdu.io.out(i)
      connections.append((smi_id("Muxn")(i), "Muxn", 0, smi_id("RDU")(0), "RDU", i))
      connections.append((smi_id("RDU")(0), "RDU", i, smi_id("IOController")(0), "IOController", i))
    }else{
      ioCtrl.io.in(i) := imuxs(i).out
      connections.append((smi_id("Muxn")(i), "Muxn", 0, smi_id("IOController")(0), "IOController", i))
    }
    offset += num
  }
  apply("connections", connections.zipWithIndex.map{case (c, i) => i -> c}.toMap)

  // configuration memory
  val ioCtrlCfgWidth = ioCtrl.io.config.getWidth
  val rduCfgWidth = { if(numRDUs > 0) rdu.io.config.getWidth else 0 }// RDU Config width
  val imuxCfgWidthList = imuxs.map{ mux => mux.config.getWidth } // input Muxes
  val imuxCfgWidth = imuxCfgWidthList.sum
  val sumCfgWidth = ioCtrlCfgWidth + rduCfgWidth + imuxCfgWidth

  val cfg = Module(new ConfigMem(sumCfgWidth, 1, cfgDataWidth))
  cfg.io.cfg_en := io.cfg_en && (cfgBlkIndex.U === io.cfg_addr(cfgAddrWidth-1, cfgBlkOffset))
  cfg.io.cfg_addr := io.cfg_addr(cfgBlkOffset-1, 0)
  cfg.io.cfg_data := io.cfg_data
  assert(cfg.cfgAddrWidth <= cfgBlkOffset)
  assert(cfgBlkIndex < (1 << (cfgAddrWidth-cfgBlkOffset)))
  val cfgOut = cfg.io.out(0)

  // ======= configuration attribute ========//
  val configuration = mutable.Map( // id : type, high, low
    smi_id("This")(0) -> ("This", sumCfgWidth-1, 0)
  )
  // IO Controller config IDs
  val ioc_cfg_id: mutable.Map[String, Int] = mutable.Map()
  ioCtrl.io.config := cfgOut(ioCtrlCfgWidth-1, 0)
  //  configuration += smi_id("IOController")(0) -> ("IOController", ioCtrlCfgWidth-1, 0)
  offset = ioCtrlCfgWidth
  ioCtrl.ioc_cfg_idx.foreach{ case (key, (offset, high, low)) =>
    configuration += (next_smi_id+offset) -> (key, high, low)
    ioc_cfg_id += key -> (next_smi_id+offset)
  }
  apply("io_controller_cfg_id", ioc_cfg_id)

  offset = ioCtrlCfgWidth
  if(rduCfgWidth != 0){
    rdu.io.config := cfgOut(rduCfgWidth+offset-1, offset)
    configuration += smi_id("RDU")(0) -> ("RDU", rduCfgWidth+offset-1, offset)
    offset += rduCfgWidth
  }
  imuxCfgWidthList.zipWithIndex.foreach{ case (w, i) =>
    if(w != 0){
      imuxs(i).config := cfgOut(w+offset-1, offset)
      configuration += smi_id("Muxn")(i) -> ("Muxn", w+offset-1, offset)
    } else {
      imuxs(i).config := DontCare
    }
    offset += w
  }

  apply("configuration", configuration)
//  apply("io_controller_cfg_format", ("[isStore, latency, baseAddr]"))

}


// object VerilogGen extends App {
//   val attrs: mutable.Map[String, Any] = mutable.Map(
//     "data_width" -> 32,
//     "cfg_data_width" -> 32,
//     "cfg_addr_width" -> 12,
//     "cfg_blk_index" -> 0,
//     "cfg_blk_offset" -> 2,
//     "iob_index" -> 0,
//     "addr_width_sram" -> 12,
//     "has_mask_sram" -> false,
//     "add_reg_sram" -> false,
//     "iob_mode" -> SRAM_MODE,
//     "lg_max_lat" -> 8,
//     "lg_max_ii" -> 4,
//     "lg_max_stride" -> 10,
//     "lg_max_cycles" -> 10,
//     "ag_nest_levels" -> 3,
//     "max_delay" -> 4,
//     "num_input_list" -> ListBuffer.fill(2){2}
//   )
//
//   (new chisel3.stage.ChiselStage).emitVerilog(new IOB(attrs),args)
// }