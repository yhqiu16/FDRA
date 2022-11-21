package tram.dsa

import chisel3._
import chisel3.util._


/** Configuration Memory (Registers)
 * 
 * @param regWidth        register data width
 * @param regNum          register number
 * @param cfgDataWidth    config data width
 */ 
class ConfigMem(regWidth: Int, regNum: Int, cfgDataWidth: Int) extends Module {
  if(regNum > 1) {
    assert(cfgDataWidth % regWidth == 0)
  }
  val cfgAddrWidth = log2Ceil((regWidth * regNum + cfgDataWidth - 1) / cfgDataWidth)

  val io = IO(new Bundle {
    val cfg_en = Input(Bool())
    val cfg_addr = Input(UInt(cfgAddrWidth.W))
    val cfg_data = Input(UInt(cfgDataWidth.W))
    val out = Output(Vec(regNum, UInt(regWidth.W)))
  })

  // configuration registers
  val regs = RegInit(VecInit(Seq.fill(regNum){0.U(regWidth.W)}))

  if(cfgDataWidth >= regWidth){
    val step = cfgDataWidth / regWidth
    for(i <- 0 until regNum){
      when(io.cfg_en && (io.cfg_addr === (i/step).U)){
        regs(i) := io.cfg_data(((i%step)+1)*regWidth-1, (i%step)*regWidth)
      }   
    }
  } else{
    val num = (regWidth + cfgDataWidth - 1) / cfgDataWidth
    for(i <- 0 until num){
      when(io.cfg_en && (io.cfg_addr === i.U)){
        if(i == 0){
          regs(0) := Cat(regs(0)(regWidth-1, (i+1)*cfgDataWidth), io.cfg_data)
        }else if(i == num-1){
          regs(0) := Cat(io.cfg_data, regs(0)(i*cfgDataWidth-1, 0))
        }else{
          regs(0) := Cat(regs(0)(regWidth-1, (i+1)*cfgDataWidth), io.cfg_data, regs(0)(i*cfgDataWidth-1, 0))
        }
      }   
    }
  }
  
  // wire out all registers
  io.out.zipWithIndex.map{ case (y, x) => y := regs(x) }
}



