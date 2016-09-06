// Created by Verilog::EditFiles from mkSMAdapter4B.v





module SizedFIFO_c (wciS0_Clk, wsiS_reqFifo__ENQ,wsiS_reqFifo__DEQ, wsiS_reqFifo__D_OUT,wsiS_reqFifo__FULL_N,wsiS_reqFifo__EMPTY_N, wsiS1_MCmd, wsiS1_MReqLast, wsiS1_MBurstPrecise, wsiS1_MBurstLength, wsiS1_MData, wsiS1_MByteEn, wsiS1_MReqInfo);


input wciS0_Clk;
////
wire	wsiS_reqFifo__CLR;
assign wsiS_reqFifo__CLR = 1'b0 ;
////
input wsiS_reqFifo__ENQ;
input wsiS_reqFifo__DEQ;

output [60:0]	wsiS_reqFifo__D_OUT;
output	wsiS_reqFifo__FULL_N;
output	wsiS_reqFifo__EMPTY_N;

wire [60:0] wsiS_reqFifo__D_IN;
input  [2 : 0] wsiS1_MCmd;
input  wsiS1_MReqLast;
input  wsiS1_MBurstPrecise;
input  [11 : 0] wsiS1_MBurstLength;
input  [31 : 0] wsiS1_MData;
input  [3 : 0] wsiS1_MByteEn;
input  [7 : 0] wsiS1_MReqInfo;



wire fulln;
wire emptyn;

wire always_one;
wire always_zero;

assign always_one = 1'b1;
assign always_zero = 1'b0;

////

assign wsiS_reqFifo__D_IN =
	     { wsiS1_MCmd,
	       wsiS1_MReqLast,
	       wsiS1_MBurstPrecise,
	       wsiS1_MBurstLength,
	       wsiS1_MData,
	       wsiS1_MByteEn,
	       wsiS1_MReqInfo } ;
////
generic_fifo_sc_c fifo_1
(.clk(wciS0_Clk),
 .rst(always_one),
 .clr (wsiS_reqFifo__CLR),
 .din (wsiS_reqFifo__D_IN),
 .we (wsiS_reqFifo__ENQ),
 .dout (wsiS_reqFifo__D_OUT),
 .re (wsiS_reqFifo__DEQ),
 .full_r (wsiS_reqFifo__FULL_N),
 .empty_r(wsiS_reqFifo__EMPTY_N),
 .full_n_r (fulln),
 .empty_n_r (emptyn)
 );








endmodule

// Created by Verilog::EditFiles from mkSMAdapter4B.v








/////////////////////////////////////////////////////////////////////
////                                                             ////
////  Universal FIFO Single Clock                                ////
////                                                             ////
////                                                             ////
////  Author: Rudolf Usselmann                                   ////
////          rudi@asics.ws                                      ////
////                                                             ////
////                                                             ////
////  D/L from: http://www.opencores.org/cores/generic_fifos/    ////
////                                                             ////
/////////////////////////////////////////////////////////////////////
////                                                             ////
//// Copyright (C) 2000-2002 Rudolf Usselmann                    ////
////                         www.asics.ws                        ////
////                         rudi@asics.ws                       ////
////                                                             ////
//// This source file may be used and distributed without        ////
//// restriction provided that this copyright statement is not   ////
//// removed from the file and that any derivative work contains ////
//// the original copyright notice and the associated disclaimer.////
////                                                             ////
////     THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY     ////
//// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED   ////
//// TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS   ////
//// FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL THE AUTHOR      ////
//// OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,         ////
//// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES    ////
//// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE   ////
//// GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR        ////
//// BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF  ////
//// LIABILITY, WHETHER IN  CONTRACT, STRICT LIABILITY, OR TORT  ////
//// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT  ////
//// OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE         ////
//// POSSIBILITY OF SUCH DAMAGE.                                 ////
////                                                             ////
/////////////////////////////////////////////////////////////////////

//  CVS Log
//
//  __Id: generic_fifo_sc_a.v,v 1.1.1.1 2002-09-25 05:42:06 rudi Exp __
//
//  __Date: 2002-09-25 05:42:06 __
//  __Revision: 1.1.1.1 __
//  __Author: rudi __
//  __Locker:  __
//  __State: Exp __
//
// Change History:
//               __Log: not supported by cvs2svn __
//
//
//
//
//
//
//
//
//
//


/*

Description
===========

I/Os
----
rst	low active, either sync. or async. master reset (see below how to select)
clr	synchronous clear (just like reset but always synchronous), high active
re	read enable, synchronous, high active
we	read enable, synchronous, high active
din	Data Input
dout	Data Output

full	Indicates the FIFO is full (combinatorial output)
full_r	same as above, but registered output (see note below)
empty	Indicates the FIFO is empty
empty_r	same as above, but registered output (see note below)

full_n		Indicates if the FIFO has space for N entries (combinatorial output)
full_n_r	same as above, but registered output (see note below)
empty_n		Indicates the FIFO has at least N entries (combinatorial output)
empty_n_r	same as above, but registered output (see note below)

level		indicates the FIFO level:
		2'b00	0-25%	 full
		2'b01	25-50%	 full
		2'b10	50-75%	 full
		2'b11	%75-100% full

combinatorial vs. registered status outputs
-------------------------------------------
Both the combinatorial and registered status outputs have exactly the same
synchronous timing. Meaning they are being asserted immediately at the clock
edge after the last read or write. The combinatorial outputs however, pass
through several levels of logic before they are output. The registered status
outputs are direct outputs of a flip-flop. The reason both are provided, is
that the registered outputs require quite a bit of additional logic inside
the FIFO. If you can meet timing of your device with the combinatorial
outputs, use them ! The FIFO will be smaller. If the status signals are
in the critical pass, use the registered outputs, they have a much smaller
output delay (actually only Tcq).

Parameters
----------
The FIFO takes 3 parameters:
dw	Data bus width
aw	Address bus width (Determines the FIFO size by evaluating 2^aw)
n	N is a second status threshold constant for full_n and empty_n
	If you have no need for the second status threshold, do not
	connect the outputs and the logic should be removed by your
	synthesis tool.

Synthesis Results
-----------------
In a Spartan 2e a 8 bit wide, 8 entries deep FIFO, takes 85 LUTs and runs
at about 116 MHz (IO insertion disabled). The registered status outputs
are valid after 2.1NS, the combinatorial once take out to 6.5 NS to be
available.


Misc
----
This design assumes you will do appropriate status checking externally.

IMPORTANT ! writing while the FIFO is full or reading while the FIFO is
empty will place the FIFO in an undefined state.

*/


// Selecting Sync. or Async Reset
// ------------------------------
// Uncomment one of the two lines below. The first line for
// synchronous reset, the second for asynchronous reset

//`define SC_FIFO_ASYNC_RESET				// Uncomment for Syncr. reset
//`define SC_FIFO_ASYNC_RESET	or negedge rst		// Uncomment for Async. reset

/*
parameter dw=8;
parameter aw=8;
parameter n=32;
parameter max_size = 1<<aw;
 */




module generic_fifo_sc_c(clk, rst, clr, din, we, dout, re,
			 full_r, empty_r,
			 full_n_r, empty_n_r);
 /*
parameter dw=8;
parameter aw=8;
parameter n=32;
parameter max_size = 1<<aw;
 */
input			clk, rst, clr;
input	[61-1:0]	din;
input			we;
output	[61-1:0]	dout;
input			re;
output			full, full_r;
output			empty, empty_r;
output			full_n, full_n_r;
output			empty_n, empty_n_r;
output	[1:0]		level;

////////////////////////////////////////////////////////////////////
//
// Local Wires
//

reg	[3-1:0]	wp;
wire	[3-1:0]	wp_pl1;
wire	[3-1:0]	wp_pl2;
reg	[3-1:0]	rp;
wire	[3-1:0]	rp_pl1;
reg			full_r;
reg			empty_r;
reg			gb;
reg			gb2;
reg	[3:0]		cnt;
wire			full_n, empty_n;
reg			full_n_r, empty_n_r;

////////////////////////////////////////////////////////////////////
//
// Memory Block
//
 wire always_zero;
 assign always_zero = 1'b0;
 wire [61-1:0] junk_out;

 wire [61-1:0] junk_in;

 // manually assign
 assign junk_in = 61'b0000000000000000000000000000000000000000000000000000000000000;

dual_port_ram   ram1(
	.clk(		clk		),
	.addr1(		rp		),
	.addr2(		wp		),
	.we1(		we		),
	.we2(		always_zero		),
	.out1(		dout		),
	.out2(		junk_out		),
	.data1(		din		),
	.data2 (	junk_in)
	);

////////////////////////////////////////////////////////////////////
//
// Misc Logic
//

always @(posedge clk )
	if(!rst)	wp <=  {3'b000};
	else
	if(clr)		wp <=  {3'b000};
	else
	if(we)		wp <=  wp_pl1;

assign wp_pl1 = wp + { {2'b00}, 1'b1};
assign wp_pl2 = wp +  { {1'b0}, 2'b10};

always @(posedge clk )
	if(!rst)	rp <=  {3'b000};
	else
	if(clr)		rp <=  {3'b000};
	else
	if(re)		rp <=  rp_pl1;

assign rp_pl1 = rp + { {2'b00}, 1'b1};

////////////////////////////////////////////////////////////////////
//
// Combinatorial Full & Empty Flags
//

assign empty = ((wp == rp) & !gb);
assign full  = ((wp == rp) &  gb);

// Guard Bit ...
always @(posedge clk )
	if(!rst)			gb <=  1'b0;
	else
	if(clr)				gb <=  1'b0;
	else
	if((wp_pl1 == rp) & we)		gb <=  1'b1;
	else
	if(re)				gb <=  1'b0;

////////////////////////////////////////////////////////////////////
//
// Registered Full & Empty Flags
//

// Guard Bit ...
always @(posedge clk )
	if(!rst)			gb2 <=  1'b0;
	else
	if(clr)				gb2 <=  1'b0;
	else
	if((wp_pl2 == rp) & we)		gb2 <=  1'b1;
	else
	if((wp != rp) & re)		gb2 <=  1'b0;

always @(posedge clk )
	if(!rst)				full_r <=  1'b0;
	else
	if(clr)					full_r <=  1'b0;
	else
	if(we & ((wp_pl1 == rp) & gb2) & !re)	full_r <=  1'b1;
	else
	if(re & ((wp_pl1 != rp) | !gb2) & !we)	full_r <=  1'b0;

always @(posedge clk )
	if(!rst)				empty_r <=  1'b1;
	else
	if(clr)					empty_r <=  1'b1;
	else
	if(we & ((wp != rp_pl1) | gb2) & !re)	empty_r <=  1'b0;
	else
	if(re & ((wp == rp_pl1) & !gb2) & !we)	empty_r <=  1'b1;

////////////////////////////////////////////////////////////////////
//
// Combinatorial Full_n & Empty_n Flags
//

assign empty_n = cnt < 32;
assign full_n  = !(cnt < (30-32+1));
assign level = {{cnt[3]}, {cnt[3]}} | cnt[3-1:3-2];

// N entries status
always @(posedge clk )
	if(!rst)	cnt <=  {3'b000};
	else
	if(clr)		cnt <=  {3'b000};
	else
	if( re & !we)	cnt <=  cnt + { 4'b1111};
	else
	if(!re &  we)	cnt <=  cnt + { {3'b000}, 1'b1};

////////////////////////////////////////////////////////////////////
//
// Registered Full_n & Empty_n Flags
//

always @(posedge clk )
	if(!rst)				empty_n_r <=  1'b1;
	else
	if(clr)					empty_n_r <=  1'b1;
	else
	if(we & (cnt >= (32-1) ) & !re)		empty_n_r <=  1'b0;
	else
	if(re & (cnt <= 32 ) & !we)		empty_n_r <=  1'b1;

always @(posedge clk )
	if(!rst)				full_n_r <=  1'b0;
	else
	if(clr)					full_n_r <=  1'b0;
	else
	if(we & (cnt >= (30-32) ) & !re)	full_n_r <=  1'b1;
	else
	if(re & (cnt <= (30-32+1)) & !we)	full_n_r <=  1'b0;

endmodule

