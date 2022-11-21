package tram.ir
// Architecture Description IR

import scala.io.Source
import java.io._
import com.fasterxml.jackson.core.`type`.TypeReference
import com.fasterxml.jackson.databind.json.JsonMapper
import com.fasterxml.jackson.databind.SerializationFeature
import com.fasterxml.jackson.module.scala.DefaultScalaModule
import scala.collection.mutable



object IRHandler{
  /** Load JSON-format IR file into metadata
   *  @param filename   IR file name
   */
  def loadIR(filename: String): mutable.Map[String,Any]={
    val json = Source.fromFile(filename)
    // parse JSON with Jackson
    val mapper = JsonMapper.builder().addModule(DefaultScalaModule).build()
    val jsonMap = mapper.readValue(json.reader(), new TypeReference[mutable.Map[String, Any]]{})
    jsonMap
  }

  /** Dump JSON-format IR file from metadata
   *  @param filename   IR file name
   */
  def dumpIR(metadata: mutable.Map[String,Any], filename: String)={
    // dump JSON with Jackson
    val mapper = JsonMapper.builder().enable(SerializationFeature.INDENT_OUTPUT)
      .addModule(DefaultScalaModule).build()
    val jsonStr = mapper.writeValueAsString(metadata)
    // write file
    val pw = new PrintWriter(new File(filename))
    pw.write(jsonStr)
    pw.close
  }
}



//object IRHandlerTest extends App {
//  val inFilename = "src/main/resources/cgra_spec.json"
//  val jsonMap = IRHandler.loadIR(inFilename)
//  println(jsonMap)
////  val outFilename = "src/main/resources/cgra_spec_w.json"
////  IRHandler.dumpIR(jsonMap, outFilename)
//}