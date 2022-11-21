//package dsa
//
//import scala.io.Source
//import java.io._
//import java.math.BigInteger
//import scala.math.BigInt
//import chisel3._
//import chisel3.util._
//
//import scala.collection.mutable
//import scala.collection.mutable.{ArrayBuffer, ListBuffer}
//import chisel3.iotesters.{ChiselFlatSpec, Driver, PeekPokeTester}
//import spec.CGRASpec
//
//class CGRATest(c: CGRA, cfgFilename: String) extends PeekPokeTester(c) {
//  val rows = c.rows
//  val cols = c.cols
//  // read config bit file
//  Source.fromFile(cfgFilename).getLines().foreach{
//    line => {
//      val items = line.split(" ")
//      val addr = Integer.parseInt(items(0), 16);        // config bus address
//      val data = BigInt(new BigInteger(items(1), 16));  // config bus data
//      poke(c.io.cfg_en, 1)
//      poke(c.io.cfg_addr, addr)
//      poke(c.io.cfg_data, data)
//      step(1)
//    }
//  }
//  // delay for config done
//  step(c.cfgRegNum + 2)
//  poke(c.io.cfg_en, 0)
//  // enable computation
//  for( i <- 0 until 2*cols){
//    poke(c.io.en(i), 1)
//  }
//  // input test data
//  // refer to mapped_dfgio.txt for the occupied I/O ports
//  for( i <- 1 until 20){
//    poke(c.io.in(4), i)
////    poke(c.io.in(0), (2*i+1))
//    step(1)
//    val valid1 = { if(i >= 4) "V: " else "X: " }
//    val valid2 = { if(i >= 6) "V: " else "X: " }
//    val out1 = peek(c.io.out(1)).toString()
//    val out2 = peek(c.io.out(6)).toString()
////    println(peek(c.io.out(1)).toString())
//    println(valid1 + out1 + ", " + valid2 + out2)
//  }
//}
//
//
//object CGRATester extends App {
//  val jsonFile = "src/main/resources/cgra_spec_1.json"
//  CGRASpec.loadSpec(jsonFile)
//  // val dut = new CGRA(attrs)
//  iotesters.Driver.execute(args, () => new CGRA(CGRASpec.attrs, false)) {
//    c => new CGRATest(c, "../bechmarks/test2/conv3/config.bit")
//  }
//
//}
//
//
