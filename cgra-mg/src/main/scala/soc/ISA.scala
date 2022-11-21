package tram

import chisel3._

object TramISA{
  // funct7 valued for different commands
  val FENCE_CMD = 0.U
  val LOAD_CMD = 1.U
  val STORE_CMD = 2.U
  val CFG_CMD = 3.U
  val EXE_CMD = 4.U
//  val CGRA_CFG_CMD = 3.U
//  val CGRA_EXE_CMD = 4.U
  val FLUSH_CMD = 5.U

  // Task ID for dependency analysis
  // One task may consist of load, cgra_cfg, cgra_exe, and store commands
  val TASK_ID_WIDTH = 5
  val DEP_TYPE_WIDTH = 3 // command dependence type
  val LD_DEP_ST_LAST_TASK = 1.U     // this load command depends on the store command of last task
  val LD_DEP_EX_LAST_TASK = 2.U     // this load command depends on the execute command of last task
  val LD_DEP_ST_LAST_SEC_TASK = 3.U // this load command depends on the store command of last second task
  val EX_DEP_ST_LAST_TASK = 1.U     // this EXECUTE command depends on the store command of last task

  // LOAD_CMD RS1
  val LOAD_CMD_RS1_REMOTE_ADDR_WIDTH = 64
  class LoadRs1 extends Bundle {
    val remote_addr = UInt(LOAD_CMD_RS1_REMOTE_ADDR_WIDTH.W)
  }

  // LOAD_CMD RS2
  val LOAD_CMD_RS2_SPAD_ADDR_WIDTH = 32
  val LOAD_CMD_RS2_TOTAL_LEN_WIDTH = 20
  val LOAD_CMD_RS2_UNUSE0_WIDTH = 2
  class LoadRs2 extends Bundle {
    val dep_type = UInt(DEP_TYPE_WIDTH.W)
    val task_id = UInt(TASK_ID_WIDTH.W)
    val is_cfg = Bool()
    val _unuse0 = UInt(LOAD_CMD_RS2_UNUSE0_WIDTH.W)
    val fused = UInt(1.W) // can be fused with the next load command, with the same remote_addr, total_len
    val total_len = UInt(LOAD_CMD_RS2_TOTAL_LEN_WIDTH.W)
    val spad_addr = UInt(LOAD_CMD_RS2_SPAD_ADDR_WIDTH.W)
  }

  // STORE_CMD RS1
  val STORE_CMD_RS1_REMOTE_ADDR_WIDTH = 64
  class StoreRs1 extends Bundle {
    val remote_addr = UInt(LOAD_CMD_RS1_REMOTE_ADDR_WIDTH.W)
  }

  // STORE_CMD RS2
  val STORE_CMD_RS2_SPAD_ADDR_WIDTH = 32
  val STORE_CMD_RS2_TOTAL_LEN_WIDTH = 20
  val STORE_CMD_RS2_UNUSE0_WIDTH = 4
  class StoreRs2 extends Bundle {
    val dep_type = UInt(DEP_TYPE_WIDTH.W) // unused
    val task_id = UInt(TASK_ID_WIDTH.W)
    val _unuse0 = UInt(STORE_CMD_RS2_UNUSE0_WIDTH.W)
    val total_len = UInt(LOAD_CMD_RS2_TOTAL_LEN_WIDTH.W)
    val spad_addr = UInt(LOAD_CMD_RS2_SPAD_ADDR_WIDTH.W)
  }

//  // CFG_CMD RS1
//  val CFG_CMD_RS1_IOB_ENS_WIDTH = 64
//  class CfgRs1 extends Bundle {
//    val iob_ens = UInt(CFG_CMD_RS1_IOB_ENS_WIDTH.W)
//  }

  // CFG_CMD RS2
  val CFG_CMD_RS2_BASE_ADDR_WIDTH = 32
  val CFG_CMD_RS2_NUM_WIDTH = 16
  val CFG_CMD_RS2_UNUSE0_WIDTH = 8
  class CfgRs2 extends Bundle {
    val dep_type = UInt(DEP_TYPE_WIDTH.W) // unused
    val task_id = UInt(TASK_ID_WIDTH.W)
    val _unuse0 = UInt(CFG_CMD_RS2_UNUSE0_WIDTH.W)
    val cfg_num = UInt(CFG_CMD_RS2_NUM_WIDTH.W)
    val cfg_base_addr = UInt(CFG_CMD_RS2_BASE_ADDR_WIDTH.W)
  }

  // EXE_CMD RS1
  val EXE_CMD_RS1_IOB_ENS_WIDTH = 64
  class ExeRs1 extends Bundle {
    val iob_ens = UInt(EXE_CMD_RS1_IOB_ENS_WIDTH.W)
  }

  // EXE_CMD RS2
//  val EXE_CMD_RS2_BASE_ADDR_WIDTH = 32
//  val EXE_CMD_RS2_NUM_WIDTH = 16
  val EXE_CMD_RS2_UNUSE0_WIDTH = 56
  class ExeRs2 extends Bundle {
    val dep_type = UInt(DEP_TYPE_WIDTH.W)
    val task_id = UInt(TASK_ID_WIDTH.W)
    val _unuse0 = UInt(EXE_CMD_RS2_UNUSE0_WIDTH.W)
//    val cfg_num = UInt(EXE_CMD_RS2_NUM_WIDTH.W)
//    val cfg_base_addr = UInt(EXE_CMD_RS2_BASE_ADDR_WIDTH.W)
  }

//  // CGRA_CFG_CMD RS1
//  val CGRA_CFG_RS1_IOB_ENS_WIDTH = 64
//  class CgraCfgRs1 extends Bundle {
//    val iob_ens = UInt(CGRA_CFG_RS1_IOB_ENS_WIDTH.W)
//  }
//
//  // CGRA_CFG_CMD RS2
//  val CGRA_CFG_RS2_BASE_ADDR_WIDTH = 16
//  val CGRA_CFG_RS2_NUM_WIDTH = 16
//  val CGRA_CFG_RS2_LOOP_CYCLES_WIDTH = 16
//  val CGRA_CFG_RS2_II_WIDTH = 8
//  class CgraCfgRs2 extends Bundle {
//    val task_id = UInt(TASK_ID_WIDTH.W)
//    val ii = UInt(CGRA_CFG_RS2_II_WIDTH.W)
//    val loop_cycles = UInt(CGRA_CFG_RS2_LOOP_CYCLES_WIDTH.W)
//    val cfg_num = UInt(CGRA_CFG_RS2_NUM_WIDTH.W)
//    val cfg_base_addr = UInt(CGRA_CFG_RS2_BASE_ADDR_WIDTH.W)
//  }
//
//  // CGRA_EXE_CMD RS1
////  val CGRA_EXE_RS1_EXE_CYCLES_WIDTH = 32
//  val CGRA_EXE_RS1_UNUSE0_WIDTH = 64
//  class CgraExeRs1 extends Bundle {
//    val _unuse0 = UInt(CGRA_EXE_RS1_UNUSE0_WIDTH.W)
////    val exe_cycles = UInt(CGRA_EXE_RS1_EXE_CYCLES_WIDTH.W)
//  }
//
//  // CGRA_EXE_CMD RS2
//  val CGRA_EXE_RS2_UNUSE0_WIDTH = 56
//  class CgraExeRs2 extends Bundle {
//    val task_id = UInt(TASK_ID_WIDTH.W)
//    val _unuse0 = UInt(CGRA_EXE_RS2_UNUSE0_WIDTH.W)
//  }


}