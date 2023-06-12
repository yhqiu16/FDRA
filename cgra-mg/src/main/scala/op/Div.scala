package tram.op

// divisor

import chisel3._
import chisel3.util._
import tram.common.CompileMacroVar._


/** True dual port SRAM
	* @param width    data width
	* @param signed   signed or unsigned
	* @param latency  output latency
	*/
class Div(width: Int, signed: Boolean, latency: Int) extends Module {
	val io = IO(new Bundle {
		val op0 = Input(UInt(width.W))
		val op1 = Input(UInt(width.W))
		val quo = Output(UInt(width.W))
		val rem = Output(UInt(width.W))
	})
	if (TARGET_TYPE == TARGET_ASIC) {
		class DW_div_pipe extends BlackBox(Map(
			"a_width" -> width,
			"b_width" -> width,
			"tc_mode" -> {
				if (signed) 1 else 0
			},
			"rem_mode" -> 1,
			"num_stages" -> (latency + 1),
			"stall_mode" -> 0,
			"rst_mode" -> 0
		)) {
			val io = IO(new Bundle {
				val clk = Input(Clock())
				val rst_n = Input(Bool())
				val en = Input(Bool())
				val a = Input(UInt(width.W))
				val b = Input(UInt(width.W))
				val quotient = Output(UInt(width.W))
				val remainder = Output(UInt(width.W))
				val divide_by_0 = Output(Bool())
			})
		}

		val div = Module(new DW_div_pipe)
		div.io.clk := clock
		div.io.rst_n := !reset.asBool
		div.io.en := true.B
		div.io.a := io.op0
		div.io.b := io.op1
		io.quo := div.io.quotient
		io.rem := div.io.remainder
	}else if (TARGET_TYPE == TARGET_FPGA) {
		class FPGA_div_pipe extends BlackBox{
			val io = IO(new Bundle {
				val aclk = Input(Clock())
				val s_axis_divisor_tvalid = Input(Bool())
				val s_axis_divisor_tdata = Input(UInt(width.W))
				val s_axis_dividend_tvalid = Input(Bool())
				val s_axis_dividend_tdata = Input(UInt(width.W))
				val m_axis_dout_tvalid = Output(Bool())
				val m_axis_dout_tdata = Output(UInt((2*width).W))
			})
		}

		val div = Module(new FPGA_div_pipe)
		div.io.aclk := clock
		div.io.s_axis_dividend_tvalid := true.B
		div.io.s_axis_dividend_tdata := io.op0
		div.io.s_axis_divisor_tvalid := true.B
		div.io.s_axis_divisor_tdata := io.op1
		io.quo := div.io.m_axis_dout_tdata(2*width-1, width)
		io.rem := div.io.m_axis_dout_tdata(width-1, 0)
	} else {
		val op0 = ShiftRegister(io.op0, latency)
		val op1 = ShiftRegister(io.op1, latency)
		when(op1 === 0.U){
			if(signed){
				io.quo := (op0.asSInt / op1.asSInt).asUInt
				io.rem := (op0.asSInt % op1.asSInt).asUInt
			}else{
				io.quo := op0.asUInt / op1.asUInt
				io.rem := op0.asUInt % op1.asUInt
			}
		}.otherwise{
			io.quo := 0.U
			io.rem := 0.U
		}
	}
}