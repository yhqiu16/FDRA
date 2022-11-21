package tram.dsa

import chisel3._
import chisel3.util._


/** reconfigurable multiplexer
 * 
 * @param width   data width
 * @param numIn   number of inputs
 */ 
class Muxn(width: Int, numIn: Int) extends Module {
  val io = IO(new Bundle{
    val config = Input(UInt(log2Ceil(numIn).W))
    val in = Input(Vec(numIn, UInt(width.W)))
    val out = Output(UInt(width.W))
  })

  val mapTable = (0 until numIn).map{ i => i.U -> io.in(i)}
  io.out := MuxLookup(io.config, io.in(0), mapTable)

}


// object VerilogGen extends App {
//   (new chisel3.stage.ChiselStage).emitVerilog(new Muxn(2, 1),args)
// }
