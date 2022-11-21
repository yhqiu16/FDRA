package tram.dsa

import chisel3._
import chisel3.util._
import scala.math.pow

/** Look-Up Table, configurate multiple entries in parallel
 * 
 * @param dataWidth       data width of the table entry
 * @param addrWidth       address width of the table
 */ 
class LUT(dataWidth: Int, addrWidth: Int) extends Module {
  val cfgDataWidth = dataWidth * pow(2,addrWidth).toInt
  val io = IO(new Bundle {
    val en = Input(Bool())
    val config = Input(UInt(cfgDataWidth.W)) 
    val in = Input(UInt(addrWidth.W))
    val out = Output(UInt(dataWidth.W))
  })

  val table = Wire(Vec(pow(2,addrWidth).toInt, UInt(dataWidth.W))) 
  table.zipWithIndex.map{ case (t, i) => t := io.config((i+1)*dataWidth-1, i*dataWidth)}

  io.out := Mux(io.en, table(io.in), 0.U)

}


// object VerilogGen extends App {
//   (new chisel3.stage.ChiselStage).emitVerilog(new LUT(2, 4),args)
// }
