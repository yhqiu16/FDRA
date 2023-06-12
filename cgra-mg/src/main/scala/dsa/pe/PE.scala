package tram.dsa

import chisel3._
import chisel3.util._
import scala.collection.mutable
import scala.collection.mutable.{ListBuffer, ArrayBuffer}
import scala.math.{pow}
import tram.op._
import tram.ir._

//// GPE: Generic Processing Element
//// GPE attributes
//trait GpeAttrs {
//  // data width
//  val width : Int
//  // cfgParams
//  val cfgDataWidth : Int
//  val cfgAddrWidth : Int
//  val cfgBlkIndex : Int   // configuration index of this block, cfg_addr[width-1 : offset]
//  val cfgBlkOffset : Int  // configuration offset bit of blocks
//}

/** GPE: Generic Processing Element
 * 
 * @param attrs     module attributes
 */ 
class GPE(attrs: mutable.Map[String, Any]) extends Module with IR {
//  apply(attrs)
  val width = attrs("data_width").asInstanceOf[Int]
  // cfgParams
  val cfgDataWidth = attrs("cfg_data_width").asInstanceOf[Int]
  val cfgAddrWidth = attrs("cfg_addr_width").asInstanceOf[Int]
  val cfgBlkIndex  = attrs("cfg_blk_index").asInstanceOf[Int]     // configuration index of this block, cfg_addr[width-1 : offset]
  val cfgBlkOffset = attrs("cfg_blk_offset").asInstanceOf[Int]   // configuration offset bit of blocks
  // number of registers in Regfile
//  val numRegRF = attrs("num_rf_reg").asInstanceOf[Int]
  // supported operations
  val opsStr = attrs("operations").asInstanceOf[ListBuffer[String]]
  val ops = opsStr.map(OPC.withName(_))
  val hasAcc = ops.map(OpInfo.isAccumulative(_) > 0).reduce(_|_) // has accumulate operations
  val aluOperandNum = ops.map(OpInfo.getOperandNum(_)).max
  // val aluCfgWidth = log2Ceil(OPC.numOPC) // ALU Config width
  val numInPerOperand = attrs("num_input_per_operand").asInstanceOf[ListBuffer[Int]]
  // max delay cycles of the RDU
  val maxDelay = attrs("max_delay").asInstanceOf[Int]

  val lgMaxWI = { if(hasAcc) attrs("lg_max_wi").asInstanceOf[Int] else 0 }              // log2(max writing Interval)
  val lgMaxLat = { if(hasAcc) attrs("lg_max_lat").asInstanceOf[Int] else 0 }            // log2(max starting latency)
  val lgMaxCycles = { if(hasAcc) attrs("lg_max_cycles").asInstanceOf[Int] else 0 }      // log2(max writing cycles)
  val lgMaxRepeats = { if(hasAcc) attrs("lg_max_repeats").asInstanceOf[Int] else 0 }    // log2(max repeat number)

  apply("data_width", width)
  apply("num_operands", aluOperandNum)
  apply("num_input", numInPerOperand.sum)
  apply("num_output", 1)
  apply("cfg_blk_index", cfgBlkIndex)
//  apply("num_rf_reg", numRegRF)
  apply("operations", opsStr)
  apply("max_delay", maxDelay)


  val io = IO(new Bundle {
    val cfg_en = Input(Bool())
    val cfg_addr = Input(UInt(cfgAddrWidth.W))
    val cfg_data = Input(UInt(cfgDataWidth.W))
    val start = Input(Bool()) // pulse signal, should be valid before latency 0, namely -1
    val en = Input(Bool())
    val in = Input(Vec(numInPerOperand.sum, UInt(width.W)))   
    val out = Output(Vec(1, UInt(width.W))) 
  })

  val alu = Module(new ALU(width, ops))
//  val rf = Module(new RF(width, numRegRF, 1, 2))
  val dmr = Module(new DualModeReg(width, hasAcc, lgMaxWI, lgMaxLat, lgMaxCycles, lgMaxRepeats))
  val rdu = Module(new SharedRDU(width, maxDelay, aluOperandNum))
  //  val rdus = Array.fill(aluOperandNum){ Module(new RDU(width, maxDelay)).io }
  val const = Wire(UInt(width.W))
  val extraMuxIns = {
    if(hasAcc && aluOperandNum > 1){
      2 // const + REG
    }else{
      1 // const
    }
  }
  val imuxs = numInPerOperand.map{ num => Module(new Muxn(width, num+extraMuxIns)).io } // input + const + (rf_out)


  // ======= sub_module attribute ========//
  // 1 : Constant
  // 2-n : sub-modules
  val sm_id: Map[String, Int] = Map(
    "Const" -> 1,
    "ALU" -> 2,
    "REG" -> 3,
    "RDU" -> 4,
    "Muxn" -> 5
  )

  val sub_modules = sm_id.map{case (name, id) => Map(
    "id" -> id,
    "type" -> name
  )}
  apply("sub_modules", sub_modules)

  // ======= sub_module instance attribute ========//
  // 0 : this module
  // 1 : Constant
  // 2-n : sub-modules
  val smi_id: Map[String, List[Int]] = Map(
    "This" -> List(0),
    "Const" -> List(1),
    "ALU" -> List(2),
    "REG" -> List(3),
    "RDU" -> List(4), // (4 until aluOperandNum+4).toList,
    "Muxn" -> (5 until aluOperandNum+5).toList // (aluOperandNum+4 until 2*aluOperandNum+4).toList
  )
  val next_smi_id : Int = aluOperandNum+5
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
  val connections = ListBuffer(
    (smi_id("ALU")(0), "ALU", 0, smi_id("REG")(0), "REG", 0),
    (smi_id("REG")(0), "REG", 0, smi_id("This")(0), "This", 0)
  )
  rdu.io.en := io.en
//  dmr.io.en := io.en
  dmr.io.in := alu.io.out
  io.out(0) := dmr.io.out(0)
  val isAccOp = RegInit(false.B)
  if(hasAcc){
    dmr.io.start := io.start
    isAccOp := OpInfo.isAccOp(alu.io.config)
    dmr.io.bypass := !isAccOp
  }else{
    dmr.io.start := DontCare
    dmr.io.bypass := DontCare
  }

  var offset = 0
  for(i <- 0 until aluOperandNum){
    val num = numInPerOperand(i)
    imuxs(i).in.zipWithIndex.foreach{ case (in, j) =>
      if(j == 0) {
        in := const
        connections.append((smi_id("Const")(0), "Const", 0, smi_id("Muxn")(i), "Muxn", j))
      } else if(j <= num) {
        in := io.in(offset+j-1)
        connections.append((smi_id("This")(0), "This", offset+j-1, smi_id("Muxn")(i), "Muxn", j))
      } else {
        in := dmr.io.out(1)
        connections.append((smi_id("REG")(0), "REG", 1, smi_id("Muxn")(i), "Muxn", j))
      }
    }
    rdu.io.in(i) := imuxs(i).out
    alu.io.in(i) := rdu.io.out(i)
    connections.append((smi_id("Muxn")(i), "Muxn", 0, smi_id("RDU")(0), "RDU", i))
    connections.append((smi_id("RDU")(0), "RDU", i, smi_id("ALU")(0), "ALU", i))
    //    rdus(i).en := io.en
    //    rdus(i).in := imuxs(i).out
    //    alu.io.in(i) := rdus(i).out
    offset += num
  }
  if(hasAcc && aluOperandNum == 1){
    alu.io.in(1) := dmr.io.out(1)
    connections.append((smi_id("REG")(0), "REG", 1, smi_id("ALU")(0), "ALU", 1))
  }

  // apply("connections", connections)
  apply("connections", connections.zipWithIndex.map{case (c, i) => i -> c}.toMap)

  // ======= configuration attribute ========//
  // configuration memory
  val constCfgWidth = width // constant
  val aluCfgWidth = alu.io.config.getWidth // ALU Config width
  val rfCfgWidth = dmr.io.config.getWidth  // REG
  //  val rduCfgWidthEach = rdus(0).config.getWidth // RDU Config width
  //  val rduCfgWidth = aluOperandNum * rduCfgWidthEach
  val rduCfgWidth = rdu.io.config.getWidth
  val imuxCfgWidthList = imuxs.map{ mux => mux.config.getWidth } // input Muxes
  val imuxCfgWidth = imuxCfgWidthList.sum
  val sumCfgWidth = constCfgWidth + aluCfgWidth + rfCfgWidth + rduCfgWidth + imuxCfgWidth

  val cfg = Module(new ConfigMem(sumCfgWidth, 1, cfgDataWidth))
  cfg.io.cfg_en := io.cfg_en && (cfgBlkIndex.U === io.cfg_addr(cfgAddrWidth-1, cfgBlkOffset))
  cfg.io.cfg_addr := io.cfg_addr(cfgBlkOffset-1, 0)
  cfg.io.cfg_data := io.cfg_data
  assert(cfg.cfgAddrWidth <= cfgBlkOffset)
  assert(cfgBlkIndex < (1 << (cfgAddrWidth-cfgBlkOffset)))

  val configuration = mutable.Map( // id : type, high, low
    smi_id("This")(0) -> ("This", sumCfgWidth-1, 0),
    smi_id("Const")(0) -> ("Const", constCfgWidth-1, 0)
  )

  val cfgOut = Wire(UInt(sumCfgWidth.W))
  cfgOut := cfg.io.out(0)
  const := cfgOut(constCfgWidth-1, 0)
  offset = constCfgWidth
  if(aluCfgWidth != 0){
    alu.io.config := cfgOut(offset+aluCfgWidth-1, offset)
    configuration += smi_id("ALU")(0) -> ("ALU", offset+aluCfgWidth-1, offset)
  } else {
    alu.io.config := DontCare
  }
  offset += aluCfgWidth

//  if(rfCfgWidth != 0){
//    dmr.io.config := cfgOut(offset+rfCfgWidth-1, offset)
//    configuration += smi_id("REG")(0) -> ("REG", offset+rfCfgWidth-1, offset)
//  } else{
//    dmr.io.config := DontCare
//  }
//  offset += rfCfgWidth
  //  for(i <- 0 until aluOperandNum){
  //    if(rduCfgWidthEach != 0){
  //      rdus(i).config := cfgOut(offset+rduCfgWidthEach-1, offset)
  //    } else {
  //      rdus(i).config := DontCare
  //    }
  //    offset += rduCfgWidthEach
  //  }
  if(rduCfgWidth != 0){
    rdu.io.config := cfgOut(offset+rduCfgWidth-1, offset)
    configuration += smi_id("RDU")(0) -> ("RDU", offset+rduCfgWidth-1, offset)
  } else{
    rdu.io.config := DontCare
  }
  if(hasAcc && (aluOperandNum > 1)){
    when(isAccOp){
      val spCfgWidth = rdu.cfgWidth // single port config width
      if(aluOperandNum > 2){
        rdu.io.config := Cat(cfgOut(offset+rduCfgWidth-1, offset+2*spCfgWidth), 0.U(spCfgWidth.W), cfgOut(offset+spCfgWidth-1, offset))
      }else{
        rdu.io.config := Cat(0.U(spCfgWidth.W), cfgOut(offset+spCfgWidth-1, offset))
      }
    }
  }
  offset += rduCfgWidth

  for(i <- 0 until aluOperandNum){
    if(imuxCfgWidthList(i) != 0){
      imuxs(i).config := cfgOut(offset+imuxCfgWidthList(i)-1, offset)
      configuration += smi_id("Muxn")(i) -> ("Muxn", offset+imuxCfgWidthList(i)-1, offset)
    } else {
      imuxs(i).config := DontCare
    }
    offset += imuxCfgWidthList(i)
  }
  if(hasAcc && aluOperandNum > 1){
    when(isAccOp){
      imuxs(1).config := (imuxs(1).in.size - 1).asUInt
    }
  }

  if(rfCfgWidth != 0){
    dmr.io.config := cfgOut(offset+rfCfgWidth-1, offset)
    // AffineCtrlReg Config Ids
    val acr_cfg_id: mutable.Map[String, Int] = mutable.Map()
    dmr.cfg_idx.foreach{ case (key, (idx, high, low)) =>
      configuration += (next_smi_id+idx) -> (key, high+offset, low+offset)
      acr_cfg_id += key -> (next_smi_id+idx)
    }
    apply("affine_ctrl_reg_cfg_id", acr_cfg_id)
  } else{
    dmr.io.config := DontCare
  }
  offset += rfCfgWidth

  apply("configuration", configuration)

}






// object VerilogGen extends App {
//   val attrs: mutable.Map[String, Any] = mutable.Map(
//     "data_width" -> 32,
//     "cfg_data_width" -> 32,
//     "cfg_addr_width" -> 8,
//     "cfg_blk_index" -> 1,
//     "cfg_blk_offset" -> 4,
//     "num_rf_reg" -> 1,
//     "operations" -> ListBuffer("ADD", "SUB", "ACC", "ASUB"),
//     "num_input_per_operand" -> ListBuffer(4, 4),
//     "max_delay" -> 7,
//     "lg_max_lat" -> 8,
//     "lg_max_wi" -> 10,
//     "lg_max_cycles" -> 10,
//   )
//
//   (new chisel3.stage.ChiselStage).emitVerilog(new GPE(attrs),args)
// }