 package tram.spec
 // Architecture Specification

 import scala.collection.mutable
 import scala.collection.mutable.ListBuffer
 import tram.ir._
 import tram.common.MacroVar._


 // CGRA Specification
 object CGRASpec{
   val cgra_gib_connect_flexibility = mutable.Map(
     "num_itrack_per_ipin" -> 2, // ipin number = 2
     "num_otrack_per_opin" -> 2, // opin number = 1
     "num_ipin_per_opin"   -> 4
   )
   val attrs: mutable.Map[String, Any] = mutable.Map(
     // 1.1. CGRA parameters
     "cgra_num_rows" -> 4,    // number of PE rows
     "cgra_num_colums" -> 4,  // number of PE columns
     "cgra_data_width" -> 32,
     "cgra_cfg_data_width" -> 32, // config bus: data width
     "cgra_cfg_addr_width" -> 10,  // config bus: address width
     "cgra_cfg_blk_offset" -> 2,  // config bus: block index offset in the address
     "cgra_gpe_num_rf_reg" -> 1,
     "cgra_gpe_operations" -> ListBuffer("PASS", "ADD", "SUB", "MUL", "AND", "OR", "XOR"),
     "cgra_gpe_max_delay" -> 4,
     "cgra_gib_num_track" -> 1,
     "cgra_gib_track_reged_mode" -> 1,
     "cgra_gib_connect_flexibility" -> cgra_gib_connect_flexibility,
     "cgra_gib_diag_iopin_connect" -> true,
     "cgra_iob_num_sides" -> 2,   // now only support top/bottom side
     "cgra_iob_mode" -> SRAM_MODE,        // 0: FIFO mode, 1: SRAM mode
     "cgra_iob_max_delay" -> 4,   // only valid for SRAM Mode
     "cgra_iob_lg_max_lat" -> 8, // log2(max in/out latency)
     "cgra_iob_lg_max_ii" -> 4,
     "cgra_iob_lg_max_stride" -> 10,
     "cgra_iob_lg_max_cycles" -> 10,
     "cgra_iob_ag_nest_levels" -> 3,
     "cgra_iob_sram_add_reg" -> 2, // add pipeline register into the SRAM IF to improve timing, write/read latency, 0 : 0/1; 1 : 1/2; 2 : 1/3;
     "cgra_iob_sram_has_mask" -> false, // byte mask in the SRAM IF
     "cgra_iob_sram_addr_width" -> 12, // address in byte
     // 1.2. CGRA Config controller parameters
     "cgra_cfg_addr_width_align" -> 16, // cfg_data and cfg_addr are stored as an array in scratchpad, cfg_addr_width should be aligned
     "cgra_cfg_sram_data_width" -> 64, // the same as the scratchpad data width
     "cgra_cfg_sram_addr_width" -> 12, // address in byte
     "cgra_cfg_sram_add_reg" -> false, // add pipeline register into the SRAM IF to improve timing
//     "cgra_cfg_sram_read_latency" -> 1, // TODO change to private
     // 1.3. CGRA Execute controller parameters
//     "cgra_exe_lg_max_ii" -> 4,
//     "cgra_exe_lg_max_loop_cycles" -> 10,
//     "cgra_exe_lg_max_execute_cycles" -> 16,
      // 2. misc parameters
      "dumpOperationSet" -> true,
      "dumpADG" -> true,
      "operation_set_filename" -> "operations.json",
      "cgra_adg_filename" -> "cgra_adg.json"
   )

   def loadSpec(jsonFile : String): Unit ={
     val jsonMap = IRHandler.loadIR(jsonFile)
     for(kv <- jsonMap){
       if(attrs.contains(kv._1)){
         if(kv._1 == "cgra_gpe_operations") {
           attrs(kv._1) = kv._2.asInstanceOf[List[String]].to[ListBuffer]
         }else if(kv._1 == "cgra_gib_connect_flexibility"){
           attrs(kv._1) = mutable.Map() ++ kv._2.asInstanceOf[Map[String, Int]]
         }else{
           attrs(kv._1) = kv._2
         }
       }
     }
   }

   def dumpSpec(jsonFile : String): Unit={
     IRHandler.dumpIR(attrs, jsonFile)
   }

 }


