package tram.dsa

import chisel3._
import chisel3.util._
import scala.collection.mutable
import scala.collection.mutable.{ArrayBuffer, ListBuffer}
import tram.op._
import tram.ir._
import tram.common.MacroVar._

/** CGRA module
 * @param attrs     module attributes
 */ 
class CGRA(attrs: mutable.Map[String, Any]) extends Module with IR{
  // CGRA parameters
  val param = CgraParam(attrs)
  import param._
//
//  val rows = attrs("cgra_num_rows").asInstanceOf[Int]     // PE number in a row
//  val cols = attrs("cgra_num_colums").asInstanceOf[Int]   // PE number in a colum
//  val dataWidth = attrs("cgra_data_width").asInstanceOf[Int] // data width in bit
//  // cfgParams
//  val cfgDataWidth = attrs("cgra_cfg_data_width").asInstanceOf[Int]
//  val cfgAddrWidth = attrs("cgra_cfg_addr_width").asInstanceOf[Int]
//  val cfgBlkOffset = attrs("cgra_cfg_blk_offset").asInstanceOf[Int]   // configuration offset bit of blocks
//
//  // ====== GPE-Specific attributes =======//
//  // number of registers in Regfile
////  val numRegRF = attrs("cgra_gpe_num_rf_reg").asInstanceOf[Int]
//  // supported operations
//  val opsStr = attrs("cgra_gpe_operations").asInstanceOf[ListBuffer[String]]
//  val ops = opsStr.map(OPC.withName(_))
//  val aluOperandNum = ops.map(OpInfo.getOperandNum(_)).max
//  // max delay cycles of the RDU in GPE
//  val maxDelayGpe = attrs("cgra_gpe_max_delay").asInstanceOf[Int]
//
//  // ====== GIB-Specific attributes =======//
//  val numTrack = attrs("cgra_gib_num_track").asInstanceOf[Int]
//  // trackRegedMode, 0: no reg; 1: half of GIBs + GIBs connected to IOBs reged; 2: all GIBs reged
//  val trackRegedMode = attrs("cgra_gib_track_reged_mode").asInstanceOf[Int]
//  // fcList
//  val fcList = attrs("cgra_gib_connect_flexibility").asInstanceOf[List[Int]]
////  val fcMap = attrs("cgra_gib_connect_flexibility").asInstanceOf[mutable.Map[String, Int]]
//  // val fci  = fcMap("num_itrack_per_ipin")     // ipin-itrack connection flexibility, connected track number
//  // val fco  = fcMap("num_otrack_per_opin")     // opin-otrack connection flexibility, connected track number
//  // val fcio = fcMap("num_ipin_per_opin")       // opin-ipin  connection flexibility, connected ipin number
//  // if support diagonal connections between OPins and IPins
//  val diagPinConect = attrs("cgra_gib_diag_iopin_connect").asInstanceOf[Boolean]
//
//  // ====== IOB-Specific attributes =======//
//  val addrWidthSram = attrs("cgra_iob_sram_addr_width").asInstanceOf[Int] - log2Ceil(dataWidth/8)   // address width (+1 every data width)
//  val hasMaskSram = false // attrs("cgra_iob_sram_has_mask").asInstanceOf[Boolean]  // if has write data byte mask
//  val addRegSram = attrs("cgra_iob_sram_add_reg").asInstanceOf[Int]        // write/read latency, 0 : 0/1; 1 : 1/2; 2 : 1/3;
//  val iobMode = attrs("cgra_iob_mode").asInstanceOf[Int]                   // 0: FIFO mode (with din, dout), 1: SRAM mode (with addr, din, dout)
//  val numIOBSides = attrs("cgra_iob_num_sides").asInstanceOf[Int]          // IOB in one/two sides
//  val agNestLevels = attrs("cgra_iob_ag_nest_levels").asInstanceOf[Int]    // nested levels of the address generation
//  val maxDelayIob = attrs("cgra_iob_max_delay").asInstanceOf[Int]          // max delay cycles of the RDU in IOB
//  val numInIOCtrl = { if(iobMode == FIFO_MODE) 1 else 2 }
//  val numIOB = numIOBSides*cols
//  val iobModeNames = Map(
//    FIFO_MODE -> "FIFO_MODE",
//    SRAM_MODE -> "SRAM_MODE"
//  )
//  val lgMaxLat = attrs("cgra_lg_max_lat").asInstanceOf[Int]            // log2(max in/out latency)
//  val lgMaxII = attrs("cgra_lg_max_ii").asInstanceOf[Int]              // log2(max in/out Initialization Interval)
//  val lgMaxStride = attrs("cgra_lg_max_stride").asInstanceOf[Int]      // log2(max address stride, n represent n*dataWidth bits)
//  val lgMaxCycles = attrs("cgra_lg_max_cycles").asInstanceOf[Int]      // log2(max in/out cycles)

  apply("num_row", rows)
  apply("num_colum", cols)
  apply("data_width", dataWidth)
  apply("cfg_data_width", cfgDataWidth)
  apply("cfg_addr_width", cfgAddrWidth)
  apply("cfg_blk_offset", cfgBlkOffset)
//  apply("gib_num_track", numTrack)                // for debug
//  apply("gib_connect_flexibility", fcMap)         // for debug
//  apply("gib_diag_iopin_connect", diagPinConect)  // for debug
  apply("num_input", numIOB)  // represented by IOB
  apply("num_output", numIOB)
  apply("iob_mode_names", iobModeNames)
  apply("iob_ag_nest_levels", agNestLevels)
//  if(iobMode == SRAM_MODE){
//  val load_latency = addRegSram + 1
//  val store_latency = {if(addRegSram > 0) 1 else 0}
////  apply("load_latency", load_latency)
////  apply("store_latency", store_latency)
////  }
//  val iob_to_spad_banks = mutable.Map[Int, List[Int]]() // the scratchpad banks connected to each IOB
//  var spad_bank_lg_size : Int = 0
//  var cfg_spad_data_width : Int = cfgDataWidth
//  if(attrs.contains("cgra_iob_sram_banks_coalesce")){
//    // divide all the banks into n groups where the internal banks are coalesced
//    // eg. {0, 1}, {2, 3}, {4, 5},...
//    val coalesceBanksIOB = attrs("cgra_iob_sram_banks_coalesce").asInstanceOf[Int]
//    for(i <- 0 until numIOB by coalesceBanksIOB){
//      val coalBanks = coalesceBanksIOB min (numIOB-i) // last group may have banks no more than coalesceBanks
//      val range = (i until (i+coalBanks)).toList
//      range.foreach{ x =>
//        iob_to_spad_banks += x -> range
//      }
//    }
//    spad_bank_lg_size = attrs("spad_bank_lg_size").asInstanceOf[Int]
//    val cfg_spad_lg_size = attrs("spad_cfg_lg_size").asInstanceOf[Int]
//    apply("cfg_spad_size", (1 << cfg_spad_lg_size))
//    cfg_spad_data_width = attrs("spad_data_width").asInstanceOf[Int]
//  }else{
//    (0 until numIOB).foreach{ x =>
//      iob_to_spad_banks += x -> List(x)
//    }
//    spad_bank_lg_size = attrs("cgra_iob_sram_addr_width").asInstanceOf[Int]
//  }
  apply("iob_to_spad_banks", iob_to_spad_banks)
  apply("iob_spad_bank_size", (1 << spad_bank_lg_size))
  apply("cfg_spad_size", (1 << cfg_spad_lg_size))
  apply("cfg_spad_data_width", cfg_spad_data_width)


  val io = IO(new Bundle{
    // config signals
    val cfg_en   = Input(Bool())
    val cfg_addr = Input(UInt(cfgAddrWidth.W))
    val cfg_data = Input(UInt(cfgDataWidth.W))
//    val ii = Input(UInt(lgMaxII.W)) // Initialization Interval, shared among all IOB
//    val cycles = Input(UInt(lgMaxCycles.W)) // valid in/out cycles, shared among all IOB
    val iob_ens = Input(UInt(numIOB.W)) // enable signals for every IOB
    // computing signals
    val en  = Input(Bool()) // global enable
    val start = Input(Bool()) // pulse signal, should be valid before latency 0, namely -1
    val done = Output(Bool()) // transfer done, keep true until next start
    val srams = Vec(numIOB, Flipped(new SRAMIO(dataWidth, addrWidthSram, hasMaskSram)))
  })

  val gpe_attrs: mutable.Map[String, Any] = mutable.Map(
    "data_width" -> dataWidth,
    "cfg_data_width" -> cfgDataWidth,
    "cfg_addr_width" -> cfgAddrWidth,
    "cfg_blk_index" -> 0,
    "cfg_blk_offset" -> cfgBlkOffset,
    "x" -> 0,
    "y" -> 0,
    "operations" -> ListBuffer(),
    "num_input_per_operand" -> ListBuffer(),
    "max_delay" -> 4,
    "lg_max_lat" -> lgMaxLat,
    "lg_max_wi" -> lgMaxCycles,
    "lg_max_cycles" -> lgMaxCycles,
    "lg_max_repeats" -> lgMaxCycles,
  )

  val gib_attrs: mutable.Map[String, Any] = mutable.Map(
    "data_width" -> dataWidth,
    "cfg_data_width" -> cfgDataWidth,
    "cfg_addr_width" -> cfgAddrWidth,
    "cfg_blk_index" -> 0,
    "cfg_blk_offset" -> cfgBlkOffset,
    "x" -> 0,
    "y" -> 0,
    "num_track" -> numTrack,
    "diag_iopin_connect" -> true,
    "num_iopin_list" -> mutable.Map[String, Int](),
    "connect_flexibility" -> List(2, 2, 4),
	  "track_reged" -> false,
    "track_directions" -> ListBuffer()
  )

  val iob_attrs: mutable.Map[String, Any] = mutable.Map(
    "data_width" -> dataWidth,
    "cfg_data_width" -> cfgDataWidth,
    "cfg_addr_width" -> cfgAddrWidth,
    "cfg_blk_index" -> 0,
    "cfg_blk_offset" -> cfgBlkOffset,
    "x" -> 0,
    "y" -> 0,
    "iob_index" -> 0,
    "addr_width_sram" -> addrWidthSram,
    "has_mask_sram" -> hasMaskSram,
    "add_reg_sram" -> addRegSram,
    "iob_mode" -> SRAM_MODE,
    "lg_max_lat" -> lgMaxLat,
    "lg_max_ii" -> lgMaxII,
    "lg_max_stride" -> lgMaxStride,
    "lg_max_cycles" -> lgMaxCycles,
    "ag_nest_levels" -> agNestLevels,
    "max_delay" -> 4,
    "num_input_per_operand" -> ListBuffer()
  )

  // ======= sub_modules attribute ========//
  // 1-n : sub-modules 
  val sm_id: mutable.Map[String, ListBuffer[Int]] = mutable.Map(
    "IOB" -> ListBuffer[Int](),
    "GPE" -> ListBuffer[Int](), 
    "GIB" -> ListBuffer[Int]()  
  )

  // ======= sub_module instances attribute ========//
  // 0 : this module
  // 1-n : sub-module instances 
  val smi_id: mutable.Map[String, ListBuffer[Int]] = mutable.Map(
    "This" -> ListBuffer(0),
    "IOB" -> ListBuffer[Int](),  // id = cfg_blk_idx
    "GPE" -> ListBuffer[Int](), // id = cfg_blk_idx
    "GIB" -> ListBuffer[Int]()  // id = cfg_blk_idx
  )

  // sub-module id to attribute
  val sm_id_attrs = mutable.Map[Int, mutable.Map[String, Any]]()
  // sub-module instance id to attribute
  val smi_id_attrs = mutable.Map[Int, mutable.Map[String, Any]]()

  val iobs = new ArrayBuffer[IOB]()
  val pes = new ArrayBuffer[GPE]()
  val gibs = new ArrayBuffer[GIB]()

  val row_idxs_iob = ListBuffer[Int]()
  val row_idxs_pe = ListBuffer[Int]()
  val row_idxs_gib = ListBuffer[Int]()

  row_idxs_iob += 0
  var idx = 1
  for(i <- 0 to rows){
    row_idxs_gib += idx
    idx += 1
    if(i < rows){
      row_idxs_pe += idx
      idx += 1
    }
  }
  if(numIOBSides > 1){
    row_idxs_iob += idx
    idx += 1
  }
  val totalRows = idx

  var sm_id_offset = 0
  val iob_type_modid : mutable.Map[Int , Int] = mutable.Map()
  // top/bottom row
  for(i <- 0 until numIOBSides) {
    val x = row_idxs_iob(i)
    val iob_index_base = x*(cols+1)
    for(j <- 0 until cols){
      val y = 2*j+1
      val index = iob_index_base+j+1
      iob_attrs("cfg_blk_index") = index
      iob_attrs("iob_index") = i*cols+j
      iob_attrs("x") = x
      iob_attrs("y") = y
      val iob_type = iob_posmap((i, j))
      val iob_param = iob_typemap(iob_type)
      iob_attrs("iob_mode") = iob_param.mode
      iob_attrs("num_input_per_operand") = iob_param.num_input_per_operand
      iob_attrs("max_delay") = iobsParam(i)(j).max_delay  // do not affect type decision
      iobs += Module(new IOB(iob_attrs))
      if(!iob_type_modid.contains(iob_type)){ // new IOB type
        sm_id_offset += 1
        iob_type_modid += (iob_type -> sm_id_offset)
        sm_id("IOB") += sm_id_offset
        sm_id_attrs += sm_id_offset -> iobs.last.getAttrs
      }
      val smi_id_attr: mutable.Map[String, Any] = mutable.Map(
        "module_id" -> iob_type_modid(iob_type),
        "cfg_blk_index" -> index,
        "iob_index" -> (i*cols+j),
        "max_delay" -> iobsParam(i)(j).max_delay,
        "x" -> x,
        "y" -> y
      )
      smi_id("IOB") += index
      smi_id_attrs += index -> smi_id_attr
    }
  }
//  sm_id("IOB") += sm_id_offset
//  sm_id_attrs += sm_id_offset -> iobs.last.getAttrs

  // GPE
  val gpe_type_modid : mutable.Map[Int , Int] = mutable.Map()
  for(i <- 0 until rows){
    val x = row_idxs_pe(i)
    for(j <- 0 until cols){
      val y = 2*j+1
      val index = x*(cols+1) + j + 1
      gpe_attrs("cfg_blk_index") = index
      gpe_attrs("x") = x
      gpe_attrs("y") = y
      val gpe_type = gpe_posmap((i, j))
      val gpe_param = gpe_typemap(gpe_type)
      gpe_attrs("operations") = gpe_param.operations
      gpe_attrs("num_input_per_operand") = gpe_param.num_input_per_operand
      gpe_attrs("max_delay") = gpesParam(i)(j).max_delay  // do not affect type decision
      pes += Module(new GPE(gpe_attrs))
      if(!gpe_type_modid.contains(gpe_type)){ // new GPE type
        sm_id_offset += 1
        gpe_type_modid += (gpe_type -> sm_id_offset)
        sm_id("GPE") += sm_id_offset
        sm_id_attrs += sm_id_offset -> pes.last.getAttrs
      }
      val smi_id_attr: mutable.Map[String, Any] = mutable.Map(
        "module_id" -> gpe_type_modid(gpe_type),
        "cfg_blk_index" -> index,
        "x" -> x,
        "y" -> y,
        "max_delay" -> gpesParam(i)(j).max_delay
      )
      smi_id("GPE") += index
      smi_id_attrs += index -> smi_id_attr
    }
  }
//  sm_id("GPE") += sm_id_offset
//  sm_id_attrs += sm_id_offset -> pes.last.getAttrs

  // GIB
  val gib_type_modid : mutable.Map[Int , Int] = mutable.Map()
//  val iopin_list_map = mutable.Map[mutable.Map[String, Int], Int]()
  for(i <- 0 to rows){
    for(j <- 0 to cols){
//      val num_iopin_list = mutable.Map[String, Int]()
//      num_iopin_list += "ipin_nw" -> {
//        if(i == 0 && j > 0) numInIOCtrl
//        else if(i > 0 && j > 0) aluOperandNum
//        else 0
//      }
//      num_iopin_list += "opin_nw" -> {
//        if(j > 0) 1 else 0
//      }
//      num_iopin_list += "ipin_ne" -> {
//        if(i == 0 && j < cols) numInIOCtrl
//        else if(i > 0 && j < cols) aluOperandNum
//        else 0
//      }
//      num_iopin_list += "opin_ne" -> {
//        if(j < cols) 1 else 0
//      }
//      num_iopin_list += "ipin_se" -> {
//        if(i == rows && j < cols) numInIOCtrl
//        else if(i < rows && j < cols) aluOperandNum
//        else 0
//      }
//      num_iopin_list += "opin_se" -> {
//        if(j < cols) 1 else 0
//      }
//      num_iopin_list += "ipin_sw" -> {
//        if(i == rows && j > 0) numInIOCtrl
//        else if(i < rows && j > 0) aluOperandNum
//        else 0
//      }
//      num_iopin_list += "opin_sw" -> {
//        if(j > 0) 1 else 0
//      }
      val gib_type = gib_posmap(i,j)
      val gib_param = gib_typemap(gib_type)
      val x = row_idxs_gib(i)
      val y = 2*j
      val index = x*(cols+1) + j + 1
      gib_attrs("cfg_blk_index") = index
      gib_attrs("x") = x
      gib_attrs("y") = y
      // if there are register behind the GIB
      //	    val reged = {
      //        if(trackRegedMode == 0) false
      //        else if(trackRegedMode == 2) true
      //        else (i%2 + j%2) == 1
      //      }
      val reged = gibsParam(i)(j).track_reged // gib_param.track_reged //
      gib_attrs("track_reged") = reged
      gib_attrs("num_iopin_list") = gib_param.num_iopin_list
      gib_attrs("diag_iopin_connect") = gib_param.diag_iopin_connect
      gib_attrs("connect_flexibility") = gib_param.fc_list
      gib_attrs("track_directions") = gib_param.track_directions
      gibs += Module(new GIB(gib_attrs))
//      if(!iopin_list_map.contains(num_iopin_list)){
//        iopin_list_map += num_iopin_list -> sm_id_offset
//        sm_id("GIB") += sm_id_offset
//        sm_id_attrs += sm_id_offset -> gibs.last.getAttrs
//        sm_id_offset += 1
//      }
      if(!gib_type_modid.contains(gib_type)){ // new GIB type
        sm_id_offset += 1
        gib_type_modid += (gib_type -> sm_id_offset)
        sm_id("GIB") += sm_id_offset
        sm_id_attrs += sm_id_offset -> gibs.last.getAttrs
      }
      val smi_id_attr: mutable.Map[String, Any] = mutable.Map(
        "module_id" -> gib_type_modid(gib_type),
        "cfg_blk_index" -> index,
        "x" -> x,
        "y" -> y,
		    "track_reged" -> reged
      )
      smi_id("GIB") += index
      smi_id_attrs += index -> smi_id_attr
    }
  }

  val sub_modules = sm_id.map{case (name, ids) =>
    ids.map{id => mutable.Map(
      "id" -> id, 
      "type" -> name,
      "attributes" -> sm_id_attrs(id)
    )}
  }.flatten
  apply("sub_modules", sub_modules)

  val instances = smi_id.map{case (name, ids) =>
    ids.map{id => mutable.Map(
      "id" -> id, 
      "type" -> name) ++ 
      {if(name != "This") smi_id_attrs(id) else mutable.Map[String, Any]()}
    }
  }.flatten
  apply("instances", instances)


  // ======= connections attribute ========//
  apply("connection_format", ("src_id", "src_type", "src_out_idx", "dst_id", "dst_type", "dst_in_idx"))
  // This:src_out_idx is the input index
  // This:dst_in_idx is the output index
  val connections = ListBuffer[(Int, String, Int, Int, String, Int)]()
  val portNameMap = gibs(0).portNameMap
//  val lastRowGibBaseIdx = rows*(cols+1)
  // IOB connections to GIB
  iobs.zipWithIndex.foreach { case (iob, i) =>
    iob.io.start := io.start && io.iob_ens(i)
    iob.io.en := io.en && io.iob_ens(i)
//    iob.io.ii := io.ii
//    iob.io.cycles := io.cycles
    iob.io.sram <> io.srams(i)
    if(i < cols){ // top row
      val gibIdx = i
      iob.io.in.zipWithIndex.foreach { case (in, j) =>
        if (j % 2 == 0) {
          in := gibs(gibIdx).io.ipinNE(j / 2)
          val index = gibs(gibIdx).oPortMap("ipinNE" + (j / 2).toString)
          connections.append((smi_id("GIB")(gibIdx), "GIB", index, smi_id("IOB")(i), "IOB", j))
        } else {
          in := gibs(gibIdx + 1).io.ipinNW(j / 2)
          val index = gibs(gibIdx + 1).oPortMap("ipinNW" + (j / 2).toString)
          connections.append((smi_id("GIB")(gibIdx + 1), "GIB", index, smi_id("IOB")(i), "IOB", j))
        }
      }
      iob.io.out.zipWithIndex.foreach { case (out, j) =>
        gibs(gibIdx).io.opinNE(j) := out
        gibs(gibIdx+1).io.opinNW(j) := out
        val index1 = gibs(gibIdx).iPortMap("opinNE" + j.toString)
        val index2 = gibs(gibIdx+1).iPortMap("opinNW" + j.toString)
        connections.append((smi_id("IOB")(i), "IOB", j, smi_id("GIB")(gibIdx), "GIB", index1))
        connections.append((smi_id("IOB")(i), "IOB", j, smi_id("GIB")(gibIdx+1), "GIB", index2))
      }
    } else { // buttom row
      val gibIdx = rows * (cols + 1) - cols + i
      iob.io.in.zipWithIndex.foreach { case (in, j) =>
        if (j % 2 == 0) {
          in := gibs(gibIdx).io.ipinSE(j / 2)
          val index = gibs(gibIdx).oPortMap("ipinSE" + (j / 2).toString)
          connections.append((smi_id("GIB")(gibIdx), "GIB", index, smi_id("IOB")(i), "IOB", j))
        } else {
          in := gibs(gibIdx + 1).io.ipinSW(j / 2)
          val index = gibs(gibIdx + 1).oPortMap("ipinSW" + (j / 2).toString)
          connections.append((smi_id("GIB")(gibIdx + 1), "GIB", index, smi_id("IOB")(i), "IOB", j))
        }
      }
      iob.io.out.zipWithIndex.foreach { case (out, j) =>
        gibs(gibIdx).io.opinSE(j) := out
        gibs(gibIdx+1).io.opinSW(j) := out
        val index1 = gibs(gibIdx).iPortMap("opinSE" + j.toString)
        val index2 = gibs(gibIdx+1).iPortMap("opinSW" + j.toString)
        connections.append((smi_id("IOB")(i), "IOB", j, smi_id("GIB")(gibIdx), "GIB", index1))
        connections.append((smi_id("IOB")(i), "IOB", j, smi_id("GIB")(gibIdx+1), "GIB", index2))
      }
    }
  }

  val done = iobs.zipWithIndex.map{ case (iob, i) => iob.io.done || (!io.iob_ens(i).asBool) }.reduce(_ & _)
  io.done := RegNext(done)

  // PE to GIB connections
  for(i <- 0 until rows){
    for(j <- 0 until cols){
      val idx_c = i*cols+j // center
      val idx_se = i*(cols+1)+j // in GIB's perspective
      val idx_sw = i*(cols+1)+j+1
      val idx_ne = (i+1)*(cols+1)+j
      val idx_nw = (i+1)*(cols+1)+j+1
      pes(idx_c).io.start := io.start
      pes(idx_c).io.en := io.en
      val gpe_param = gpe_typemap(gpe_posmap(i, j))
      val numOperand = gpe_param.num_input_per_operand.size // operand number
      // which directions of GIBs are connected to GPE input ports
      // number of inputs from each direction: numOperand
      val from_dir = gpesParam(i)(j).from_dir
      if(from_dir.contains(NORTHWEST)){
        val baseindex = from_dir.indexOf(NORTHWEST)
        for( k <- 0 until numOperand ){
          val indexgpe = baseindex + k*from_dir.size // input order: inputs for 1st operand, inputs for 2nd operand...
          pes(idx_c).io.in(indexgpe) := gibs(idx_se).io.ipinSE(k)
          val indexgib = gibs(idx_se).oPortMap("ipinSE" + (k).toString)
          connections.append((smi_id("GIB")(idx_se), "GIB", indexgib, smi_id("GPE")(idx_c), "GPE", indexgpe))
        }
      }
      if(from_dir.contains(NORTHEAST)){
        val baseindex = from_dir.indexOf(NORTHEAST)
        for( k <- 0 until numOperand ){
          val indexgpe = baseindex + k*from_dir.size
          pes(idx_c).io.in(indexgpe) := gibs(idx_sw).io.ipinSW(k)
          val indexgib = gibs(idx_sw).oPortMap("ipinSW" + (k).toString)
          connections.append((smi_id("GIB")(idx_sw), "GIB", indexgib, smi_id("GPE")(idx_c), "GPE", indexgpe))
        }
      }
      if(from_dir.contains(SOUTHWEST)){
        val baseindex = from_dir.indexOf(SOUTHWEST)
        for( k <- 0 until numOperand ){
          val indexgpe = baseindex + k*from_dir.size
          pes(idx_c).io.in(baseindex + k*from_dir.size) := gibs(idx_ne).io.ipinNE(k)
          val indexgib = gibs(idx_ne).oPortMap("ipinNE" + (k).toString)
          connections.append((smi_id("GIB")(idx_ne), "GIB", indexgib, smi_id("GPE")(idx_c), "GPE", indexgpe))
        }
      }
      if(from_dir.contains(SOUTHEAST)){
        val baseindex = from_dir.indexOf(SOUTHEAST)
        for( k <- 0 until numOperand ){
          val indexgpe = baseindex + k*from_dir.size
          pes(idx_c).io.in(indexgpe) := gibs(idx_nw).io.ipinNW(k)
          val indexgib = gibs(idx_nw).oPortMap("ipinNW" + (k).toString)
          connections.append((smi_id("GIB")(idx_nw), "GIB", indexgib, smi_id("GPE")(idx_c), "GPE", indexgpe))
        }
      }

      // which directions of GIBs are connected to GPE output port
      val to_dir = gpesParam(i)(j).to_dir
      pes(idx_c).io.out.zipWithIndex.foreach { case (out, k) =>
        if (to_dir.contains(NORTHWEST)) {
          gibs(idx_se).io.opinSE(k) := out
          val index = gibs(idx_se).iPortMap("opinSE" + k.toString)
          connections.append((smi_id("GPE")(idx_c), "GPE", k, smi_id("GIB")(idx_se), "GIB", index))
        }
        if (to_dir.contains(NORTHEAST)) {
          gibs(idx_sw).io.opinSW(k) := out
          val index = gibs(idx_sw).iPortMap("opinSW" + k.toString)
          connections.append((smi_id("GPE")(idx_c), "GPE", k, smi_id("GIB")(idx_sw), "GIB", index))
        }
        if (to_dir.contains(SOUTHWEST)) {
          gibs(idx_ne).io.opinNE(k) := out
          val index = gibs(idx_ne).iPortMap("opinNE" + k.toString)
          connections.append((smi_id("GPE")(idx_c), "GPE", k, smi_id("GIB")(idx_ne), "GIB", index))
        }
        if (to_dir.contains(SOUTHEAST)) {
          gibs(idx_nw).io.opinNW(k) := out
          val index = gibs(idx_nw).iPortMap("opinNW" + k.toString)
          connections.append((smi_id("GPE")(idx_c), "GPE", k, smi_id("GIB")(idx_nw), "GIB", index))
        }
      }
    }
  }

  // GIB to GIB connections
  if(numTrack > 0) {
    for (i <- 0 to rows) {
      for (j <- 0 to cols) {
        val idx_c = i * (cols + 1) + j // center
        val idx_n = (i - 1) * (cols + 1) + j // in center GIB's perspective
        val idx_w = i * (cols + 1) + j - 1
        val idx_e = i * (cols + 1) + j + 1
        val idx_s = (i + 1) * (cols + 1) + j
        if (i == 0) {
          gibs(idx_c).io.itrackN.foreach { in => in := 0.U }
          gibs(idx_c).io.itrackS.zipWithIndex.foreach { case (in, k) =>
            in := gibs(idx_s).io.otrackN(k)
            val index1 = gibs(idx_c).iPortMap("itrackS" + k.toString)
            val index2 = gibs(idx_s).oPortMap("otrackN" + k.toString)
            connections.append((smi_id("GIB")(idx_s), "GIB", index2, smi_id("GIB")(idx_c), "GIB", index1))
          }
        } else if (i == rows) {
          gibs(idx_c).io.itrackS.foreach { in => in := 0.U }
          gibs(idx_c).io.itrackN.zipWithIndex.foreach { case (in, k) =>
            in := gibs(idx_n).io.otrackS(k)
            val index1 = gibs(idx_c).iPortMap("itrackN" + k.toString)
            val index2 = gibs(idx_n).oPortMap("otrackS" + k.toString)
            connections.append((smi_id("GIB")(idx_n), "GIB", index2, smi_id("GIB")(idx_c), "GIB", index1))
          }
        } else {
          gibs(idx_c).io.itrackN.zipWithIndex.foreach { case (in, k) =>
            in := gibs(idx_n).io.otrackS(k)
            val index1 = gibs(idx_c).iPortMap("itrackN" + k.toString)
            val index2 = gibs(idx_n).oPortMap("otrackS" + k.toString)
            connections.append((smi_id("GIB")(idx_n), "GIB", index2, smi_id("GIB")(idx_c), "GIB", index1))
          }
          gibs(idx_c).io.itrackS.zipWithIndex.foreach { case (in, k) =>
            in := gibs(idx_s).io.otrackN(k)
            val index1 = gibs(idx_c).iPortMap("itrackS" + k.toString)
            val index2 = gibs(idx_s).oPortMap("otrackN" + k.toString)
            connections.append((smi_id("GIB")(idx_s), "GIB", index2, smi_id("GIB")(idx_c), "GIB", index1))
          }
        }
        if (j == 0) {
          gibs(idx_c).io.itrackW.foreach { in => in := 0.U }
          gibs(idx_c).io.itrackE.zipWithIndex.foreach { case (in, k) =>
            in := gibs(idx_e).io.otrackW(k)
            val index1 = gibs(idx_c).iPortMap("itrackE" + k.toString)
            val index2 = gibs(idx_e).oPortMap("otrackW" + k.toString)
            connections.append((smi_id("GIB")(idx_e), "GIB", index2, smi_id("GIB")(idx_c), "GIB", index1))
          }
        } else if (j == cols) {
          gibs(idx_c).io.itrackE.foreach { in => in := 0.U }
          gibs(idx_c).io.itrackW.zipWithIndex.foreach { case (in, k) =>
            in := gibs(idx_w).io.otrackE(k)
            val index1 = gibs(idx_c).iPortMap("itrackW" + k.toString)
            val index2 = gibs(idx_w).oPortMap("otrackE" + k.toString)
            connections.append((smi_id("GIB")(idx_w), "GIB", index2, smi_id("GIB")(idx_c), "GIB", index1))
          }
        } else {
          gibs(idx_c).io.itrackW.zipWithIndex.foreach { case (in, k) =>
            in := gibs(idx_w).io.otrackE(k)
            val index1 = gibs(idx_c).iPortMap("itrackW" + k.toString)
            val index2 = gibs(idx_w).oPortMap("otrackE" + k.toString)
            connections.append((smi_id("GIB")(idx_w), "GIB", index2, smi_id("GIB")(idx_c), "GIB", index1))
          }
          gibs(idx_c).io.itrackE.zipWithIndex.foreach { case (in, k) =>
            in := gibs(idx_e).io.otrackW(k)
            val index1 = gibs(idx_c).iPortMap("itrackE" + k.toString)
            val index2 = gibs(idx_e).oPortMap("otrackW" + k.toString)
            connections.append((smi_id("GIB")(idx_e), "GIB", index2, smi_id("GIB")(idx_c), "GIB", index1))
          }
        }
      }
    }
  }

  // apply("connections", connections)
  apply("connections", connections.zipWithIndex.map{case (c, i) => i -> c}.toMap)
  
  // Configurations, each row share one config bus
  val cfgRegNum = totalRows - 1
  val cfgRegs = RegInit(VecInit(Seq.fill(cfgRegNum)(0.U((1+cfgAddrWidth+cfgDataWidth).W))))
  cfgRegs(0) := Cat(io.cfg_en, io.cfg_addr, io.cfg_data)
  (1 until cfgRegNum).foreach{ i => cfgRegs(i) := cfgRegs(i-1) }
  iobs.zipWithIndex.foreach{ case (iob, j) => // top and buttom row
    if(j < cols){
      iob.io.cfg_en   := io.cfg_en
      iob.io.cfg_addr := io.cfg_addr
      iob.io.cfg_data := io.cfg_data
    }else{
      iob.io.cfg_en   := cfgRegs(cfgRegNum-1)(cfgAddrWidth+cfgDataWidth)
      iob.io.cfg_addr := cfgRegs(cfgRegNum-1)(cfgAddrWidth+cfgDataWidth-1, cfgDataWidth)
      iob.io.cfg_data := cfgRegs(cfgRegNum-1)(cfgDataWidth-1, 0)
    }
  }
  for(i <- 0 to rows){
    val gibCfgIdx = row_idxs_gib(i) - 1
    val peCfgIdx = {if(i < rows) row_idxs_pe(i) - 1 else 0}
    for(j <- 0 to cols){
      gibs(i*(cols+1)+j).io.cfg_en   := cfgRegs(gibCfgIdx)(cfgAddrWidth+cfgDataWidth)
      gibs(i*(cols+1)+j).io.cfg_addr := cfgRegs(gibCfgIdx)(cfgAddrWidth+cfgDataWidth-1, cfgDataWidth)
      gibs(i*(cols+1)+j).io.cfg_data := cfgRegs(gibCfgIdx)(cfgDataWidth-1, 0)
      if((i < rows) && (j < cols)){
        pes(i*cols+j).io.cfg_en   := cfgRegs(peCfgIdx)(cfgAddrWidth+cfgDataWidth)
        pes(i*cols+j).io.cfg_addr := cfgRegs(peCfgIdx)(cfgAddrWidth+cfgDataWidth-1, cfgDataWidth)
        pes(i*cols+j).io.cfg_data := cfgRegs(peCfgIdx)(cfgDataWidth-1, 0)
      }
    }
  }
  // config bits of the blocks
  val blkCfgBits = ListBuffer[Int]()
  blkCfgBits ++= iobs.map(_.sumCfgWidth).toList
  blkCfgBits ++= pes.map(_.sumCfgWidth).toList
  blkCfgBits ++= gibs.map(_.cfgsBit).toList
  val maxCfgDataNum = blkCfgBits.map{ x => (x+cfgDataWidth-1)/cfgDataWidth }.sum
//  println("Max cfg bits: " + blkCfgBits.max, ", Min cfg bits: " + blkCfgBits.min, ", Total cfg bits: " + blkCfgBits.sum)
  apply("max_blk_cfg_bits", blkCfgBits.max)
  apply("min_blk_cfg_bits", blkCfgBits.min)
  apply("sum_blk_cfg_bits", blkCfgBits.sum)
  apply("max_cfg_data_num", maxCfgDataNum)

  if(attrs("dumpADG").asInstanceOf[Boolean]){
    printIR(attrs("cgra_adg_filename").asInstanceOf[String])
  }
  if(attrs("dumpOperationSet").asInstanceOf[Boolean]){
    OpInfo.setLatency("INPUT", load_latency)
    OpInfo.setLatency("OUTPUT", store_latency)
    OpInfo.setLatency("LOAD", load_latency)
    OpInfo.setLatency("STORE", store_latency)
    OpInfo.dumpOpInfo(attrs("operation_set_filename").asInstanceOf[String])
  }

}





//object VerilogGen extends App {
//  val connect_flexibility = mutable.Map(
//    "num_itrack_per_ipin" -> 2, // ipin number = 3
//    "num_otrack_per_opin" -> 6, // opin number = 1
//    "num_ipin_per_opin"   -> 9
//  )
//  val attrs: mutable.Map[String, Any] = mutable.Map(
//    "num_row" -> 4,
//    "num_colum" -> 4,
//    "data_width" -> 32,
//    "cfg_data_width" -> 64,
//    "cfg_addr_width" -> 8,
//    "cfg_blk_offset" -> 2,
//    "num_rf_reg" -> 1,
//    "operations" -> ListBuffer("PASS", "ADD", "SUB", "MUL", "AND", "OR", "XOR", "SEL"),
//    "max_delay" -> 4,
//    "num_track" -> 3,
//    "connect_flexibility" -> connect_flexibility,
//    "num_output_ib" -> 3,
//    "num_input_ob" -> 6
//  )
//
//  (new chisel3.stage.ChiselStage).emitVerilog(new CGRA(attrs, true), args)
//}