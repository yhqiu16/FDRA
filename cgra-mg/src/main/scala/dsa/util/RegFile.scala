package tram.dsa

import chisel3._
import chisel3.util._
import scala.math.pow


/** Register File
 * 
 * @param width     register data width
 * @param regNum    number of registers, >= 1
 * @param numIn     number of input ports
 * @param numOut    number of output ports
 */
class RF(width: Int, regNum: Int, numIn: Int, numOut: Int) extends Module {
  val selWidth = log2Ceil(regNum) // width of the register selecting signal
  val io = IO(new Bundle {
    val en = Input(Bool())
    val config = Input(UInt((selWidth*(numIn+numOut)).W)) // 0,1,2...: in(0).sel, in(1).sel,..., out(0).sel, out(1).sel,...
    val in = Input(Vec(numIn, UInt(width.W)))
    val out = Output(Vec(numOut, UInt(width.W)))
  })

  val regs = RegInit(VecInit(Seq.fill(regNum){0.U(width.W)}))
  if(regNum > 1){
    val sels = Wire(Vec(numIn+numOut, UInt(selWidth.W))) 
    (0 until (numIn+numOut)).map{ i => sels(i) := io.config((i+1)*selWidth-1, i*selWidth)}
  
    when(io.en){
      (0 until numIn).map{ i => regs(sels(i)) := io.in(i) }
    } 
    (numIn until (numIn+numOut)).map{ i => io.out(i-numIn) := regs(sels(i)) }
  } else{ // regNum == 1
    when(io.en){
      (0 until numIn).map{ i => regs(0) := io.in(i) }
    }
    (0 until numOut).map{ i => io.out(i) := regs(0) }
  }
  
}


