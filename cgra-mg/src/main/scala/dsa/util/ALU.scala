package tram.dsa

import chisel3._
import chisel3.util._
import scala.collection.mutable.ListBuffer
import tram.op._


/** reconfigurable arithmetic unit
 * 
 * @param width   data width
 */
class ALU(width: Int, ops: ListBuffer[OPC.OPC]) extends Module {
  val op_info = OpInfo(width)
  val maxNumOperands = ops.map(OpInfo(width).getALUOperandNum(_)).max
  val cfgDataWidth = log2Ceil(OPC.numOPC)
  val io = IO(new Bundle {
    val config = Input(UInt(cfgDataWidth.W))
    val in = Input(Vec(maxNumOperands, UInt(width.W)))
    val out = Output(UInt(width.W)) 
  })

  val op_func_map = op_info.OpFuncs(io.in.toSeq, io.config, ops)
  val op2res = ops.map{op => 
    (op.id.U -> op_func_map(op.toString)(0))
  }

  io.out := MuxLookup(io.config, 0.U, op2res)

}


// object VerilogGen extends App {
//   (new chisel3.stage.ChiselStage).emitVerilog(new ALU(64, ListBuffer(OPC.ADD, OPC.SUB, OPC.SEL)),args)
// }
