package tram.spec
// Architecture Specification

import chisel3._
import chisel3.util._

import scala.collection.mutable
import scala.collection.mutable.ListBuffer
import tram.ir._
import tram.common.MacroVar._

import java.io.File

// GPE Spec to support heterogeneous GPEs
case class GpeSpec(
  max_delay : Int = 4,    // max delay cycles of the DelayPipe
  operations : ListBuffer[String] = ListBuffer( "PASS", "ADD", "SUB"),       // supported operations
//  from_dir : List[Int] =  List(NORTHWEST, NORTHEAST, SOUTHWEST, SOUTHEAST),  // which directions the GPE inputs are from
//  to_dir : List[Int] =  List(NORTHWEST, NORTHEAST, SOUTHWEST, SOUTHEAST)     // which directions the GPE outputs are to
)

// IOB Spec to support heterogeneous IOBs
case class IobSpec(
  mode : Int = SRAM_MODE,
  max_delay : Int = 4    // max delay cycles of the DelayPipe
)

// GIB Spec to support heterogeneous GIBs
case class GibSpec(
  diag_iopin_connect : Boolean = true,  // if support diagonal connections between OPins and IPins
  fc_list: List[Int] = List(2, 2, 4)    // num_itrack_per_ipin, num_otrack_per_opin, num_ipin_per_opin
  // "num_itrack_per_ipin" : ipin-itrack connection flexibility, connected track number
  // "num_otrack_per_opin" : opin-otrack connection flexibility, connected track number
  // "num_ipin_per_opin"   : opin-ipin  connection flexibility, connected ipin number
)


// CGRA Specification
object TramSpec{
  val system_bus_beat_bits = 64 // data width of the system bus
  val spad_bank_lg_size = 13     // log2(single scratchpad bank size in byts)
  val spad_cfg_lg_size = 10      // log2(config scratchpad size in byts)
  val cgra_iob_sram_banks_coalesce = 4 // coalescing sram banks that CGRA IOB can access
//  val cgra_cfg_sram_banks_cascade = 2 // cascading sram banks that CGRA config controller can access
//  val cgra_gib_connect_flexibility = mutable.Map(
//    "num_itrack_per_ipin" -> 2, // ipin number = 2
//    "num_otrack_per_opin" -> 2, // opin number = 1
//    "num_ipin_per_opin"   -> 4
//  )
  val attrs: mutable.Map[String, Any] = mutable.Map(
    // 1. CGRA Controller parameters
    // 1.1. CGRA Global parameters
    "cgra_num_row" -> 4,    // number of PE rows
    "cgra_num_colum" -> 8,  // number of PE colums
    "cgra_data_width" -> 32,
    "cgra_cfg_data_width" -> 32, // config bus: data width
    "cgra_cfg_addr_width" -> 11,  // config bus: address width
    "cgra_cfg_blk_offset" -> 2,  // config bus: block index offset in the address
    // 1.2. GPE attributes (default for all)
//    "cgra_gpe_num_rf_reg" -> 1,
    "cgra_gpe_operations" -> ListBuffer("PASS", "ADD", "SUB", "MUL", "SHL", "LSHR", "ASHR", "ACC", "ASUB"),
    "cgra_gpe_max_delay" -> 4,
    "cgra_gpe_in_from_dir" -> List(NORTHWEST, NORTHEAST, SOUTHWEST, SOUTHEAST),
    "cgra_gpe_out_to_dir" -> List(NORTHWEST, NORTHEAST, SOUTHWEST, SOUTHEAST),
    // 1.3. GIB attributes (default for all)
    "cgra_gib_num_track" -> 1,
    "cgra_gib_track_reged_mode" -> 1,
    "cgra_gib_connect_flexibility" -> List(2, 2, 4),
    "cgra_gib_diag_iopin_connect" -> true,
    // 1.4. IOB attributes (default for all)
    "cgra_iob_num_sides" -> 2,   // now only support top/bottom side
    "cgra_iob_mode" -> SRAM_MODE,        // 0: FIFO mode, 1: SRAM mode
    "cgra_iob_max_delay" -> 4,   // only valid for SRAM Mode
    "cgra_iob_ag_nest_levels" -> 3,
    "cgra_iob_sram_add_reg" -> 2, // add pipeline register into the SRAM IF to improve timing, write/read latency, 0 : 0/1; 1 : 1/2; 2 : 1/3;
//    "cgra_iob_sram_has_mask" -> false, // byte mask in the SRAM IF
    "cgra_iob_sram_addr_width" -> (spad_bank_lg_size + log2Ceil(cgra_iob_sram_banks_coalesce)), // address in byte
    "cgra_iob_sram_banks_coalesce" -> cgra_iob_sram_banks_coalesce,
    "cgra_lg_max_lat" -> 6, // log2(max in/out latency)
    "cgra_lg_max_ii" -> 1,
    "cgra_lg_max_stride" -> 13,
    "cgra_lg_max_cycles" -> 12,
    // 1.5. CGRA Config controller parameters
    "cgra_cfg_addr_width_align" -> 16, // cfg_data and cfg_addr are stored as an array in scratchpad, cfg_addr_width should be aligned
//    "cgra_cfg_sram_banks_cascade" -> cgra_cfg_sram_banks_cascade,
//    "cgra_cfg_sram_data_width" -> 32,
//    "cgra_cfg_sram_addr_width" -> (spad_bank_lg_size + log2Ceil(cgra_cfg_sram_banks_cascade)), // address in byte
    "cgra_cfg_sram_add_reg" -> false, // add pipeline register into the SRAM IF to improve timing
//    "cgra_cfg_sram_read_latency" -> 1,
    // 1.6. CGRA Execute controller parameters
//    "cgra_exe_lg_max_ii" -> 4,
//    "cgra_exe_lg_max_loop_cycles" -> 10,
//    "cgra_exe_lg_max_execute_cycles" -> 16,
    // 2. Scratchpad parameters
    "spad_data_width" -> system_bus_beat_bits,
    "spad_bank_lg_size" -> spad_bank_lg_size,
    "spad_cfg_lg_size" -> spad_cfg_lg_size,
    "spad_addr_num" -> 5, // scratchpad address number, be multiple to support broadcast write
//    "spad_num_banks" -> (cgra_iob_num_sides * cgra_num_colums + cgra_cfg_sram_banks_cascade)
    // 3. Load/Store controller parameters
    "ls_stream_queue_depth" -> 0,  // cache stream data
    // 4. Reservation station parameters
    "id_width" -> 6, // command ID for debug
//    "rs_cmd_queue_depth" -> 16,
    "rs_load_queue_depth" -> 8,
    "rs_store_queue_depth" -> 8,
    "rs_exe_queue_depth" -> 4,   // CGRA controller queue
    // 5. DMA parameters
    "dma_num_req_in_flight" -> 8,
    "dma_lg_max_burst_size" -> 6, // max data size in bytes of one burst transferring, <=6 (limited by TileLink edge attribute)
    // 6. TLB parameters
    "tlb_num_ways" -> 32,  // way number in the set-associate tlb
    "tlb_is_shared" -> true, // TLB is shared by DMA reader and writer
    // 7. system bus parameters
    "system_bus_beat_bits" -> system_bus_beat_bits,
    // 8. misc parameters
    "dumpOperationSet" -> true,
    "dumpADG" -> true,
    "operation_set_filename" -> "operations.json",
    "cgra_adg_filename" -> "cgra_adg.json"
  )
  attrs += ("spad_num_banks" -> (attrs("cgra_iob_num_sides").asInstanceOf[Int] * attrs("cgra_num_colum").asInstanceOf[Int]))

  // set default values from attr
  // the attributes in attrs are used as default values
  def setDefaultGpesSpec(): Unit = {
    val gpes_spec = ListBuffer[ListBuffer[GpeSpec]]()
    for(i <- 0 until attrs("cgra_num_row").asInstanceOf[Int]){
      gpes_spec.append(new ListBuffer[GpeSpec])
      for( j <- 0 until attrs("cgra_num_colum").asInstanceOf[Int]){
        val max_delay = attrs("cgra_gpe_max_delay").asInstanceOf[Int]
        val operations = attrs("cgra_gpe_operations").asInstanceOf[ListBuffer[String]]
        gpes_spec(i).append(GpeSpec(max_delay, operations))
      }
    }
    attrs("cgra_gpes") = gpes_spec
  }

  def setDefaultIobsSpec(): Unit = {
    val iobs_spec = ListBuffer[ListBuffer[IobSpec]]()
    for(i <- 0 until attrs("cgra_iob_num_sides").asInstanceOf[Int]){
      iobs_spec.append(new ListBuffer[IobSpec])
      for( j <- 0 until attrs("cgra_num_colum").asInstanceOf[Int]){
        val mode = attrs("cgra_iob_mode").asInstanceOf[Int]
        val maxDelay = attrs("cgra_iob_max_delay").asInstanceOf[Int]
        iobs_spec(i).append(IobSpec(mode, maxDelay))
      }
    }
    attrs("cgra_iobs") = iobs_spec
  }

  // Coarse-grained GIBs
  def setDefaultGibsSpec(): Unit = {
    val gibs_spec = ListBuffer[ListBuffer[GibSpec]]()
    for(i <- 0 to attrs("cgra_num_row").asInstanceOf[Int]){
      gibs_spec.append(new ListBuffer[GibSpec])
      for( j <- 0 to attrs("cgra_num_colum").asInstanceOf[Int]){
        val diag_iopin_connect = attrs("cgra_gib_diag_iopin_connect").asInstanceOf[Boolean]
        val fclist = attrs("cgra_gib_connect_flexibility").asInstanceOf[List[Int]]
        gibs_spec(i).append(GibSpec(diag_iopin_connect, fclist))
      }
    }
    attrs("cgra_gibs") = gibs_spec
  }

  setDefaultGpesSpec()
  setDefaultIobsSpec()
  setDefaultGibsSpec()
  
  def loadSpec(jsonFile : String): Unit ={
    val jsonMap = IRHandler.loadIR(jsonFile)
    var gpes_spec_update = false
    var iobs_spec_update = false
    var gibs_spec_update = false
    for(kv <- jsonMap){
      if(attrs.contains(kv._1)){
        if(kv._1 == "cgra_gpe_operations") {
          attrs(kv._1) = kv._2.asInstanceOf[List[String]].to[ListBuffer]
        }else if(kv._1 == "cgra_gib_connect_flexibility"){
          attrs(kv._1) = kv._2.asInstanceOf[List[Int]]
//          attrs(kv._1) = mutable.Map() ++ kv._2.asInstanceOf[Map[String, Int]]
        } else if (kv._1 == "cgra_gpe_in_from_dir") {
          attrs(kv._1) = kv._2.asInstanceOf[List[Int]]
        } else if (kv._1 == "cgra_gpe_out_to_dir") {
          attrs(kv._1) = kv._2.asInstanceOf[List[Int]]
        } else if (kv._1 == "cgra_gpes") {
          gpes_spec_update = true
          val gpe_2d = kv._2.asInstanceOf[List[List[Any]]]
          val gpes_spec = ListBuffer[ListBuffer[GpeSpec]]()
          for (i <- gpe_2d.indices) {
            gpes_spec.append(new ListBuffer[GpeSpec])
            val gpe_1d = gpe_2d(i)
            for (j <- gpe_1d.indices) {
              val gpemap = gpe_1d(j).asInstanceOf[Map[String, Any]]
              val max_delay = gpemap("max_delay").asInstanceOf[Int]
              val operations = ListBuffer[String]() ++ gpemap("operations").asInstanceOf[List[String]]
              gpes_spec(i).append(GpeSpec(max_delay, operations))
            }
          }
          attrs("cgra_gpes") = gpes_spec
        } else if (kv._1 == "cgra_iobs") {
          iobs_spec_update = true
          val iob_2d = kv._2.asInstanceOf[List[List[Any]]]
          val iobs_spec = ListBuffer[ListBuffer[IobSpec]]()
          for (i <- iob_2d.indices) {
            iobs_spec.append(new ListBuffer[IobSpec])
            val iob_1d = iob_2d(i)
            for (j <- iob_1d.indices) {
              val iobmap = iob_1d(j).asInstanceOf[Map[String, Any]]
              val mode = iobmap("mode").asInstanceOf[Int]
              val maxDelay = iobmap("max_delay").asInstanceOf[Int]
              iobs_spec(i).append(IobSpec(mode, maxDelay))
            }
          }
          attrs("cgra_iobs") = iobs_spec
        } else if (kv._1 == "cgra_gibs") {
          gibs_spec_update = true
          val gib_2d = kv._2.asInstanceOf[List[List[Any]]]
          val gibs_spec = ListBuffer[ListBuffer[GibSpec]]()
          for (i <- gib_2d.indices) {
            gibs_spec.append(new ListBuffer[GibSpec])
            val gib_1d = gib_2d(i)
            for (j <- gib_1d.indices) {
              val gibmap = gib_1d(j).asInstanceOf[Map[String, Any]]
              val diag_iopin_connect = gibmap("diag_iopin_connect").asInstanceOf[Boolean]
              val fclist = gibmap("fc_list").asInstanceOf[List[Int]]
              gibs_spec(i).append(GibSpec(diag_iopin_connect, fclist))
            }
          }
          attrs("cgra_gibs") = gibs_spec
        }else{
          attrs(kv._1) = kv._2
        }
      }
    }
    if(gpes_spec_update == false){ // set default values
      setDefaultGpesSpec()
    }
    if(iobs_spec_update == false){ // set default values
      setDefaultIobsSpec()
    }
    if(gibs_spec_update == false) { // set default values
      setDefaultGibsSpec()
    }

    // verification
    assert(attrs("cgra_iob_sram_addr_width").asInstanceOf[Int] == attrs("spad_bank_lg_size").asInstanceOf[Int] +
      log2Ceil(attrs("cgra_iob_sram_banks_coalesce").asInstanceOf[Int]))
//    assert(attrs("cgra_cfg_sram_addr_width").asInstanceOf[Int] == attrs("spad_bank_lg_size").asInstanceOf[Int] +
//      log2Ceil(attrs("cgra_cfg_sram_banks_cascade").asInstanceOf[Int]))
//    assert(attrs("cgra_cfg_sram_data_width").asInstanceOf[Int] == attrs("system_bus_beat_bits").asInstanceOf[Int])
    assert(attrs("spad_data_width").asInstanceOf[Int] == attrs("system_bus_beat_bits").asInstanceOf[Int])
//    if(attrs("cgra_iob_mode").asInstanceOf[Int] == SRAM_MODE){
//      assert(attrs("cgra_iob_sram_add_reg").asInstanceOf[Boolean] == true)
//    }
  }

  def dumpSpec(jsonFile : String): Unit={
    IRHandler.dumpIR(attrs, jsonFile)
  }

}



object SpecGen extends App {
  val rootDirPath = (new File("")).getAbsolutePath()
  val tram_spec_filename = rootDirPath + "/generators/tram/cgra-mg/src/main/resources/tram_spec.json"
  TramSpec.dumpSpec(tram_spec_filename)
//  TramSpec.loadSpec(tram_spec_filename)
}
