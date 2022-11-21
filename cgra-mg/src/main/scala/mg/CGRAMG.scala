package tram.mg

import tram.spec.CGRASpec
import tram.op._
import tram.dsa._
import java.io.File

// TODO: add to command options
//case class Config(
//  loadSpec: Boolean = true,
//  dumpOperations: Boolean = true,
//  dumpIR: Boolean = true,
//  genVerilog: Boolean = true,
//)

// CGRA Modeling and Generation
object CGRAMG extends App{
  val loadSpec : Boolean = false
  val dumpOperationSet : Boolean = true
  val dumpADG : Boolean = true
  val genVerilog : Boolean = true
  val rootDirPath = (new File("")).getAbsolutePath()

  if(loadSpec){
    val jsonFile = rootDirPath + "/generators/tram/cgra-mg/src/main/resources/cgra_spec.json"
    CGRASpec.dumpSpec(jsonFile)
    CGRASpec.loadSpec(jsonFile)
  }
  CGRASpec.attrs("dumpOperationSet") = dumpOperationSet
  if(dumpOperationSet){
    CGRASpec.attrs("operation_set_filename") = rootDirPath + "/generators/tram/cgra-mg/src/main/resources/operations.json"
  }
  CGRASpec.attrs("dumpADG") = dumpADG
  if(dumpADG){
    CGRASpec.attrs("cgra_adg_filename") = rootDirPath + "/generators/tram/cgra-mg/src/main/resources/cgra_adg.json"
  }
  if(genVerilog){
    (new chisel3.stage.ChiselStage).emitVerilog(new CGRA(CGRASpec.attrs), args)
  }else{ // not emit verilog to speedup
    (new chisel3.stage.ChiselStage).emitChirrtl(new CGRA(CGRASpec.attrs), args)
  }
}
