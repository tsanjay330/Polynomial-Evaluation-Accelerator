/*******************************************************************************
This is the module that implements the EVB instruction.

--- Inputs ---

clk: clock

rst: reset

start_evb: enable signal for starting an EVB instruction

A: first EVB argument; index of coefficient vector array we wish to use for
   computations

b: second EVB argument; number of polynomials we wish to compute using the same
   x and coefficient vector

x_b: data value read in from data RAM to be used as x in the current computation

c_i: coefficient read in from S RAM to be used as c_i in computations

N: length of coefficient vector S[A], read in from N RAM

rd_addr_data: address (index) of data RAM

--- Outputs ---

done_evp: signal that indicates a single EVP computation has finished processing

done_evb: signal that indicates EVB is finished processing

en_rd_data: enable signal that triggers RAM to read value from data RAM

en_rd_S: enable signal that triggers RAM to read a value from S RAM

en_rd_N: enable signal that triggers RAM to read a value from N RAM

rd_addr_data_updated: updated address (index) of data RAM

rd_addr_S: address (index) of S RAM we wish to read from

rd_addr_N: address (index) of N RAM we wish to read from

result: value outputted to the result FIFO

status: value outputted to the status FIFO

--- Local ---

state: current state that the EVB module is operating in

next_state: the state that the EVB module will operate in next

en_evp: signal inputted to EVP to tell it to begin processing

next_done_evp: temporary register to hold next value of done_evp, outputted 
               from the EVP instance

next_result: temporary register to hold next value of result, outputted from
             the EVP instance

next_status: temporary register to hold next value of status, outputted from
             the EVB instance

b_counter: counter that tracks how many computations we have done so far

next_b_counter: temporary register to hold next value of b_counter

*******************************************************************************/

`timescale 1ns/1ps

module EVB_FSM_3
		#(parameter buffer_size = 1024)(
		input clk, rst,
		input start_evb,
		input [2 : 0] A,
		input [4 : 0] b,
		input [15 : 0] x_b,
		input [15 : 0] c_i,
		input [4 : 0] N,
		input [log2(buffer_size) - 1 : 0] rd_addr_data,
		output reg done_evp,
		output reg done_evb,
		output en_rd_data,
		output en_rd_S,
		output en_rd_N,
		output [log2(buffer_size) - 1 : 0] rd_addr_data_updated,
		output [6 : 0] rd_addr_S,
		output [2 : 0] rd_addr_N, 
		output reg [31 : 0] result,
		output reg [31 : 0] status);

		reg [3 : 0] state, next_state;
		reg en_evp;
		wire next_done_evp;
		wire [31 : 0] next_result, next_status;
		reg [4 : 0] b_counter, next_b_counter;

	localparam STATE_START = 4'b0000, STATE_FIRST_EVP_EN_EVP = 4'b0001, 
				STATE_FIRST_EVP_WAIT = 4'b0010, STATE_FIRST_EVP_DONE = 4'b0011,
				STATE_CHECK_B = 4'b0100, STATE_EVP_EN_EVP = 4'b0101,
				STATE_EVP_WAIT = 4'b0110, STATE_EVP_DONE = 4'b0111, 
				STATE_END = 4'b1000;

	EVP_FSM_3 evp
		(.clk(clk), .rst(rst), .start_evp(en_evp), .A(A), .ram_out_data(x_b), 
			.ram_out_S(c_i), .N(N), .rd_addr_data(rd_addr_data), 
			.en_rd_data(en_rd_data), .en_rd_S(en_rd_S), .en_rd_N(en_rd_N), 
			.rd_addr_data_updated(rd_addr_data_updated), .rd_addr_S(rd_addr_S), 
			.rd_addr_N(rd_addr_N), .done_evp(next_done_evp), 
			.result(next_result), .status(next_status));

	/* Update state, outputs, and registers that rely on a next value */
	always @(posedge clk, negedge rst)
		if (! rst) begin
			state <= STATE_START;
			b_counter <= 0;
			done_evp <= 0;
			result <= 0;
			status <= 32'b11111111111111111111111111111111;
		end
		else begin
			state <= next_state;
			b_counter <= next_b_counter;
			done_evp <= next_done_evp;
			result <= next_result;
			status <= next_status;
		end

	/* Determine the next state of the module */
	always @(*) // state, start_evb, next_done_evp, b)
		case (state)
			STATE_START:
				if (start_evb)
					next_state <= STATE_FIRST_EVP_EN_EVP;
				else
					next_state <= STATE_START;

			STATE_FIRST_EVP_EN_EVP:
				next_state <= STATE_FIRST_EVP_WAIT;

			STATE_FIRST_EVP_WAIT:
				if (next_done_evp)
					next_state <= STATE_FIRST_EVP_DONE;
				else
					next_state <= STATE_FIRST_EVP_WAIT;

			STATE_FIRST_EVP_DONE:
				next_state <= STATE_CHECK_B;

			STATE_CHECK_B:
				if (b_counter == b)
					next_state <= STATE_END;
				else
					next_state <= STATE_EVP_EN_EVP;

			STATE_EVP_EN_EVP:
				next_state <= STATE_EVP_WAIT;

			STATE_EVP_WAIT:
				if (next_done_evp)
					next_state <= STATE_EVP_DONE;
				else
					next_state <= STATE_EVP_WAIT;

			STATE_EVP_DONE:
            	next_state <= STATE_CHECK_B;

			STATE_END:
				next_state <= STATE_START;

		endcase

	/* Update outputs and registers according to the state */
	always @(*) // state, b_counter)
		case (state)
			STATE_START:
			begin
				done_evb <= 0;
				en_evp <= 0;
				next_b_counter <= 0;
			end

			STATE_FIRST_EVP_EN_EVP:
			begin
				done_evb <= 0;
				en_evp <= 1;
				next_b_counter <= b_counter;
			end

			STATE_FIRST_EVP_WAIT:
			begin
				done_evb <= 0;
				en_evp <= 0;
				next_b_counter <= b_counter;
			end

			STATE_FIRST_EVP_DONE:
			begin
				done_evb <= 0;
				en_evp <= 0;
				next_b_counter <= b_counter + 1;
			end
	
			STATE_CHECK_B:
			begin
				done_evb <= 0;
				en_evp <= 0;
				next_b_counter <= b_counter;
			end

			STATE_EVP_EN_EVP:
			begin
				done_evb <= 0;
				en_evp <= 1;
				next_b_counter <= b_counter;
			end

			STATE_EVP_WAIT:
			begin
				done_evb <= 0;
				en_evp <= 0;
				next_b_counter <= b_counter;
			end

			STATE_EVP_DONE:
			begin
				done_evb <= 0;
				en_evp <= 0;
				next_b_counter <= b_counter + 1;
			end

			STATE_END:
			begin
				done_evb <= 1;
				en_evp <= 0;
				next_b_counter <= b_counter;
			end
		endcase

	function integer log2;
	input [31 : 0] value;
	integer i;
	begin
		if(value==1)
			log2=1;
		else begin
			i = value - 1;
			for (log2 = 0; i > 0; log2 = log2 + 1) begin
				i = i >> 1;
			end
		end
	end
	endfunction

endmodule
