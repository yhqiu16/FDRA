package tram.op
// ALU/IOB operations

import chisel3._
import chisel3.util._
import scala.collection.mutable
import scala.collection.mutable.ListBuffer
import tram.ir._

/**
 * Operation Code
 */
object OPC extends Enumeration {
	type OPC = Value
	// Operation Code
	val PASS,    // passthrough from in to out
		ADD,     // add
	  SUB,     // substrate
	  MUL,     // multiply
	  UDIV,     // divide
	  // MOD,     // modulo
	  // MIN,
	  // NOT,
//	  AND,
//	  OR,
//	  XOR,
	  SHL,     // shift left
	  LSHR,    // logic shift right
		ASHR,    // arithmetic shift right
//	  CSHL,    // cyclic shift left
//	  CSHR,    // cyclic shift right
	  EQ,      // equal to
	  NE,      // not equal to
	  ULT,      // less than
	  ULE,      // less than or equal to
		SEL,
		ACC,	 // +=
		ASUB,  // -=
//	  AMUL,  // *=
//		ADIV,  // /=
//		AMOD,  // %=
//		AAND,  // &=
//		AOR,   // |=
//		AXOR,  // ^=
//		ASHL,  // <<=
//		ALSHR, // >>=
//		AASHR, // >>>=
		INPUT,
		OUTPUT,
		LOAD,
		STORE = Value

	val numOPC = this.values.size - 4 // not including INPUT/OUTPUT/LOAD/STORE, since ALU OPC config width is determined by numOPC

	def printOPC = {
		this.values.foreach{ op => println(s"$op\t: ${op.id}")}
	}
}

/**
 *  Operation Information
 */
object OpInfo {
	import OPC._
	val OpInfoMap: Map[String, ListBuffer[Int]] = Map(
		// OPC -> List(NumOperands, NumRes, Latency, Operands-Commutative, Accumulative-operation)
		// latency including the register outside ALU
		"PASS" -> ListBuffer(1, 1, 1, 0, 0),
		"ADD" -> ListBuffer(2, 1, 1, 1, 0),
		"SUB" -> ListBuffer(2, 1, 1, 0, 0),
		"MUL" -> ListBuffer(2, 1, 1, 1, 0),
		"UDIV" -> ListBuffer(2, 1, 1, 0, 0),
		"MOD" -> ListBuffer(2, 1, 1, 0, 0),
		"MIN" -> ListBuffer(2, 1, 1, 1, 0),
		"MAX" -> ListBuffer(2, 1, 1, 1, 0),
		"AND" -> ListBuffer(2, 1, 1, 1, 0),
		"OR"  -> ListBuffer(2, 1, 1, 1, 0),
		"XOR" -> ListBuffer(2, 1, 1, 1, 0),
		"SHL" -> ListBuffer(2, 1, 1, 0, 0),
		"LSHR" -> ListBuffer(2, 1, 1, 0, 0),
		"ASHR" -> ListBuffer(2, 1, 1, 0, 0),
		"CSHL" -> ListBuffer(2, 1, 1, 0, 0),
		"CSHR" -> ListBuffer(2, 1, 1, 0, 0),
		"EQ"  -> ListBuffer(2, 1, 1, 1, 0),
		"NE"  -> ListBuffer(2, 1, 1, 1, 0),
		"ULT" -> ListBuffer(2, 1, 1, 0, 0),
		"ULE" -> ListBuffer(2, 1, 1, 0, 0),
		"SLT" -> ListBuffer(2, 1, 1, 0, 0),
		"SLE" -> ListBuffer(2, 1, 1, 0, 0),
		"SEL" -> ListBuffer(3, 1, 1, 0, 0),
		"ACC"  -> ListBuffer(1, 1, 1, 0, 1),  // +=
		"ASUB" -> ListBuffer(1, 1, 1, 0, 1), // -=
		"AMUL" -> ListBuffer(1, 1, 1, 0, 1), // *=
		"ADIV" -> ListBuffer(1, 1, 1, 0, 1), // /=
		"AMOD" -> ListBuffer(1, 1, 1, 0, 1), // %=
		"AAND" -> ListBuffer(1, 1, 1, 0, 1), // &=
		"AOR"  -> ListBuffer(1, 1, 1, 0, 1),  // |=
		"AXOR" -> ListBuffer(1, 1, 1, 0, 1), // ^=
		"ASHL" -> ListBuffer(1, 1, 1, 0, 1), // <<=
		"ALSHR" -> ListBuffer(1, 1, 1, 0, 1),// >>=
		"AASHR" -> ListBuffer(1, 1, 1, 0, 1),// >>>=
		"INPUT"  -> ListBuffer(0, 1, 2, 0, 0),
		"OUTPUT" -> ListBuffer(1, 0, 1, 0, 0),
		"LOAD"   -> ListBuffer(1, 1, 2, 0, 0),
		"STORE"  -> ListBuffer(2, 0, 1, 0, 0)
	)

//	val AccOpNames : Array[String] = Array(
//		"ACC",
//		"ASUB",
//		"AMUL",
//		"ADIV",
//		"AMOD",
//		"AAND",
//		"AOR",
//		"AXOR",
//		"ASHL",
//		"ALSHR",
//		"AASHR"
//	)

	def getOperandNum(op: OPC): Int = {
		OpInfoMap(op.toString)(0)
	}

	def getALUOperandNum(op: OPC): Int = {
		val name = op.toString
		if(isAccumulative(op) > 0){
			OpInfoMap(name)(0) + 1
		}else{
			OpInfoMap(name)(0)
		}
	}

	def getResNum(op: OPC): Int = {
		OpInfoMap(op.toString)(1)
	}

	def getLatency(op: OPC): Int = {
		OpInfoMap(op.toString)(2)
	}

	def setLatency(op: String, lat: Int): Unit = {
		OpInfoMap(op)(2) = lat
	}

	def isCommutative(op: OPC.OPC): Int = {
		OpInfoMap(op.toString)(3)
	}

	def isAccumulative(op: OPC.OPC): Int = {
		OpInfoMap(op.toString)(4)
	}

	def dumpOpInfo(filename: String): Unit = {
		val infos = ListBuffer[Map[String, Any]]();
		OPC.values.foreach{ op =>
			val info = Map(
				"name" -> op.toString,
				"OPC" -> op.id,
				"numOperands" -> getOperandNum(op),
				"numRes" -> getResNum(op),
				"latency" -> getLatency(op),
				"commutative" -> isCommutative(op),
				"accumulative" ->isAccumulative(op)
			)
			infos += info
		}
		val ops: mutable.Map[String,Any] = mutable.Map("Operations" -> infos)
		IRHandler.dumpIR(ops, filename)
	}

	private var width = 32
	private var high = width - 1

	def apply(opWidth: Int) = {
		width = opWidth
		high = width - 1
		this
	}

	def isAccOp(op: UInt) : Bool = {
		val table = OPC.values.map{ op =>
			op.id.U -> (isAccumulative(op) > 0).asBool
		}.toSeq
		MuxLookup(op, false.B, table)
	}

	def OpFuncs(ops: Seq[UInt], opc: UInt, opset: ListBuffer[OPC]) : Map[String, Seq[UInt]] = {
//		val op_names = opset.map(_.toString)
		val op0 = ops.head(high, 0)
		val op1 = ops(1)(high, 0)
		val op2 = {
			if(ops.size > 2) ops(2)(0)
			else 0.U(1.W)
		}
//		val op1_inv = (~op1).asUInt
//		val op1_new = Wire(UInt(width.W))
//		val op2_new = Wire(UInt(1.W))
//		op1_new := op1
//		op2_new := 0.U(1.W)
//		if(op_names.contains("SUB")){
//			when(opc === OPC.withName("SUB").id.U){
//				op1_new := op1_inv
//				op2_new := 1.U(1.W)
//			}
//		}
//		if(op_names.contains("ASUB")){
//			when(opc === OPC.withName("ASUB").id.U){
//				op1_new := op1_inv
//				op2_new := 1.U(1.W)
//			}
//		}
//		val adder = Wire(UInt((width+1).W))
//		adder := Cat(0.U(1.W), op0) + op1_new + op2_new
		// shift number
		val shn = op1(log2Ceil(width)-1, 0)
		val shn0 = op0(log2Ceil(width)-1, 0)

		Map(
			"PASS" -> Seq(op0),
//			"ADD"  -> Seq(adder(high, 0)),
//			"SUB"  -> Seq(adder(high, 0)),
			"ADD"  -> Seq(op0 + op1),
			"SUB"  -> Seq(op0 - op1),
			"MUL"  -> Seq(op0 * op1),
			"UDIV"  -> Seq(op0 / op1),
			"MOD"  -> Seq(op0 % op1),
			"MIN"  -> Seq(Mux(op0 < op1, op0, op1)),
			"MAX"  -> Seq(Mux(op0 > op1, op0, op1)),
			"AND"  -> Seq(op0 & op1),
			"OR"   -> Seq(op0 | op1),
			"XOR"  -> Seq(op0 ^ op1),
			"SHL"  -> Seq((op0 << shn).asUInt),
			"LSHR" -> Seq((op0 >> shn).asUInt),
			"ASHR" -> Seq((op0.asSInt >> shn).asUInt),
			"CSHL" -> {
				val res1 = (op0 << shn).asUInt
				val res2 = (op0 >> (width.U - shn)).asUInt
				Seq(res1 | res2) },
			"CSHR" -> {
				val res1 = (op0 >> shn).asUInt
				val res2 = (op0 << (width.U - shn)).asUInt
				Seq(res1 | res2) },
			"EQ"   -> {
				val res = op0 === op1
				Seq(res, res)},
			"NE"   -> {
				val res = op0 =/= op1
				Seq(res, res)},
			"ULT"  -> { // unsigned less than
				val res = op0 < op1
				Seq(res, res)},
			"ULE"  -> { // unsigned less than or equal
				val res = op0 <= op1
				Seq(res, res)},
			"SLT"  -> { // signed less than
				val res = op0.asSInt < op1.asSInt
				Seq(res, res)},
			"SLE"  -> { // signed less than or equal
				val res = op0.asSInt <= op1.asSInt
				Seq(res, res)},
			"SEL"  -> {
				Seq(Mux(op2.asBool, op1, op0)) },
			"ACC"   -> Seq(op1 + op0), // the SECOND operand is the register value. the following is similar
			"ASUB"  -> Seq(op1 - op0),
			"AMUL"  -> Seq(op1 * op0),
			"ADIV"  -> Seq(op1 / op0),
			"AMOD"  -> Seq(op1 % op0),
			"AAND"  -> Seq(op1 & op0),
			"AOR"   -> Seq(op1 | op0),
			"AXOR"  -> Seq(op1 ^ op0),
			"ASHL"  -> Seq((op1 << shn0).asUInt),
			"ALSHR" -> Seq((op1 >> shn0).asUInt),
			"AASHR" -> Seq((op1.asSInt >> shn0).asUInt)
		)
	}
}


//object test extends App {
//	println(OPC.values)
//	println(s"OPC number: ${OPC.numOPC}")
//	println(OPC(0), OPC.withName("ADD"))
//	OPC.printOPC
//	val outFilename = "src/main/resources/operations.json"
//	OpInfo.dumpOpInfo(outFilename)
//	// println(OpInfo.OpFuncMap(OPC.ADD)(Seq(1.U, 2.U)))
//}