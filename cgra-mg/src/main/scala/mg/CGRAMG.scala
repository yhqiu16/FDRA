package tram.mg

import tram.spec.TramSpec
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
  val loadSpecParam : Boolean = true
  val dumpOperationSet : Boolean = true
  val dumpADG : Boolean = true
  val genVerilog : Boolean = true
  val rootDirPath = (new File("")).getAbsolutePath()

  if(loadSpecParam){
//    val jsonFile = rootDirPath + "/generators/tram/cgra-mg/src/main/resources/tram_spec.json"
//    TramSpec.dumpSpec()
    TramSpec.loadSpec()
  }
  TramSpec.attrs("dumpOperationSet") = dumpOperationSet
//  if(dumpOperationSet){
//    TramSpec.attrs("operation_set_filename") = rootDirPath + "/generators/tram/cgra-mg/src/main/resources/operations.json"
//  }
  TramSpec.attrs("dumpADG") = dumpADG
//  if(dumpADG){
//    TramSpec.attrs("cgra_adg_filename") = rootDirPath + "/generators/tram/cgra-mg/src/main/resources/cgra_adg.json"
//  }
  if(genVerilog){
    (new chisel3.stage.ChiselStage).emitVerilog(new CGRA(TramSpec.attrs), args)
  }else{ // not emit verilog to speedup
    (new chisel3.stage.ChiselStage).emitChirrtl(new CGRA(TramSpec.attrs), args)
  }
}
