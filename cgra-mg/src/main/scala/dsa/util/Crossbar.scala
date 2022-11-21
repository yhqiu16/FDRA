package tram.dsa

import chisel3._
import chisel3.util._

/** reconfigurable crossbar
 *
 * @param width   data width
 * @param numIn   number of inputs
 * @param numOut  number of outputs
 */
class Crossbar(width: Int, numIn: Int, numOut: Int) extends Module {
  val selWidth = log2Ceil(numIn)    // width of the selecting signal
  val sumCfgWidth = numOut*selWidth // total config data width
  val io = IO(new Bundle{
    val config = Input(UInt(sumCfgWidth.W))
    val in = Input(Vec(numIn, UInt(width.W)))
    val out = Output(Vec(numOut, UInt(width.W)))
  })

  if(numIn > 1){
    val muxs = Array.fill(numOut){Module(new Muxn(width, numIn)).io}
    for(i <- 0 until numOut){
      for(j <- 0 until numIn){
        muxs(i).in(j) := io.in(j)
      }
      io.out(i) := muxs(i).out
      muxs(i).config := io.config((i+1)*selWidth-1, i*selWidth)
    }
  } else{ // numIn == 1
    (0 until numOut).map{ i => io.out(i) := io.in(0) }
  }

}
