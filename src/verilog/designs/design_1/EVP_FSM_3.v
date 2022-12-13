/*******************************************************************************
This is the module that implements the EVP instruction.

--- Inputs ---

clk: clock

rst: reset

start_evp: enable signal for starting an EVP instruction

A: EVP argument; index of coefficient vector array we wish to use for 
   computations

ram_out_data: data value read in from data RAM to be used as x in 
              computations

ram_out_S: coefficient read in from S RAM to be used as c_i in computations

N: length of coefficient vector S[A], read in from N RAM

rd_addr_data: address (index) of data RAM

--- Outputs ---

en_rd_data: enable signal that triggers RAM to read value from data RAM

en_rd_S: enable signal that triggers RAM to read a value from S RAM

en_rd_N: enable signal that triggers RAM to read a value from N RAM

rd_addr_data_updated: updated address (index) of data RAM

rd_addr_S: address (index) of S RAM we wish to read from

rd_addr_N: address (index) of N RAM we wish to read from

done_evp: signal that indicates EVP is finished processing

result: value outputted to the result FIFO

status: value outputted to the status FIFO

--- Local ---

state: current state that the EVP module is operating in

next_state: the state that the EVP module will operate in next

next_result: temporary register to hold next value of result

next_status: temporary register to hold next value of status

S_idx_counter: counter for which index of S we wish to operate on, used to 
               determine rd_addr_S

next_S_idx_counter: temporary register to hold next value of S_idx_counter

monomial: computation of x to the power of the current degree

next_monomial: temporary register to hold next value of monomial

sum: sum of all previous c_i * monomial computations

next_sum: temporary register to hold next value of sum

next_rd_addr_data: temporary register to hold next value of rd_addr_data_updated

next_rd_addr_S: temporary register to hold next value of rd_addr_S

next_rd_addr_N: temporary register to hold next value of rd_addr_N

x: x in computation c_0 * x^0 + c_1 * x^1 + ... + c_N * x^N

c_i: c_i in computation c_0 * x^0 + c_1 * x^1 + ... + c_N * x^N, 0 <= i <= N

*******************************************************************************/

`timescale 1ns/1ps

module EVP_FSM_3 
		#(parameter buffer_size = 1024)(
		input clk, rst,
		input start_evp,
		input [2 : 0] A,
		input [15 : 0] ram_out_data,
		input [15 : 0] ram_out_S,
		input [4 : 0] N,
		input [log2(buffer_size)-1 : 0] rd_addr_data, 
		output reg en_rd_data,
		output reg en_rd_S,
		output reg en_rd_N,
		output reg [log2(buffer_size)-1 : 0] rd_addr_data_updated,
		output [6 : 0] rd_addr_S,
		output reg [2 : 0] rd_addr_N,
		output reg done_evp,
		output reg [31 : 0] result,
		output reg [31 : 0] status);

		reg [3 : 0] state, next_state;
		reg [31 : 0] next_result;
		reg [31 : 0] next_status;
		reg [3 : 0] S_idx_counter, next_S_idx_counter;
		reg [31 : 0] monomial, next_monomial;
		reg [31 : 0] sum, next_sum;
		reg [log2(buffer_size)-1 : 0] next_rd_addr_data;
		reg [6 : 0] next_rd_addr_S;
		reg [2 : 0] next_rd_addr_N;
   		reg [15 : 0] x;
  		reg [15 : 0] c_i;
   
   

	localparam STATE_IDLE = 4'b0000, STATE_START = 4'b0001, 
				STATE_RD_N = 4'b0010, STATE_CHECK_N = 4'b0011, 
				STATE_RD_DATA = 4'b0100, STATE_COMPUTE_SUM = 4'b0101, 
				STATE_GET_NEXT_COEFF = 4'b0110, STATE_COMPUTE_EXP = 4'b0111, 
				STATE_OUTPUT = 4'b1000, STATE_ERROR = 4'b1001, 
				STATE_END = 4'b1010;

assign rd_addr_S = A * 11 + S_idx_counter;

	/* Update state, outputs, and registers that rely on a next value */
	always @(posedge clk, negedge rst)
		if (! rst) begin
			state <= STATE_IDLE;
			rd_addr_data_updated <= 0;
			S_idx_counter <= 0;
			rd_addr_N <= 0;
			monomial <= 1;
			sum <= 0;
			result <= 0;
			status <= 32'b11111111111111111111111111111111;
		end
		else begin
			state <= next_state;
			rd_addr_data_updated <= next_rd_addr_data;
			S_idx_counter <= next_S_idx_counter;
			rd_addr_N <= next_rd_addr_N;
			monomial <= next_monomial;
			sum <= next_sum;
			result <= next_result;
			status <= next_status;
		end

	/* Determine the next state of the module */
	always @(*) // state, start_evp, S_idx_counter, N)
		case (state)
			STATE_IDLE:
				if (start_evp)
					next_state <= STATE_START;
				else
					next_state <= STATE_IDLE;

			STATE_START:
				next_state <= STATE_RD_N;

			STATE_RD_N:
				next_state <= STATE_CHECK_N;

			STATE_CHECK_N:
				if (N == 5'b11111)
					next_state <= STATE_ERROR;
				else
					next_state <= STATE_RD_DATA;

			STATE_RD_DATA:
				next_state <= STATE_COMPUTE_SUM;

			STATE_COMPUTE_SUM:
				if (S_idx_counter > N)
					next_state <= STATE_OUTPUT;
				else
					next_state <= STATE_GET_NEXT_COEFF;

			STATE_GET_NEXT_COEFF:
				next_state <= STATE_COMPUTE_EXP;

			STATE_COMPUTE_EXP:
				next_state <= STATE_COMPUTE_SUM;

			STATE_OUTPUT:
				next_state <= STATE_END;

			STATE_ERROR:
				next_state <= STATE_END;

			STATE_END:
				next_state <= STATE_IDLE;
	
		endcase

	/* Update outputs and registers according to the state */
	always @(*) // state, rd_addr_data, rd_addr_data_updated, 
				// rd_addr_N, S_idx_counter, A, c_i, sum, x,
				// monomial, result, status)
		case (state)
			STATE_IDLE:
			begin
				done_evp <= 0;
				en_rd_data <= 0;
				next_rd_addr_data <= rd_addr_data_updated;
				en_rd_S <= 0;
				next_S_idx_counter <= 10;
				en_rd_N <= 0;
				next_rd_addr_N <= A;
				next_monomial <= 1;
				next_sum <= 0;
				next_result <= 0;
				next_status <= 32'b11111111111111111111111111111111;
			end

			STATE_START:
			begin
				done_evp <= 0;
				en_rd_data <= 0;
				next_rd_addr_data <= rd_addr_data;
				en_rd_S <= 0;
				next_S_idx_counter <= 0;
				en_rd_N <= 0;
				next_rd_addr_N <= A;
				next_monomial <= 1;
				next_sum <= 0;
				next_result <= 0;
				next_status <= 32'b11111111111111111111111111111111;
			end

			STATE_RD_N:
			begin
				done_evp <= 0;
				en_rd_data <= 0;
				next_rd_addr_data <= rd_addr_data_updated;
				en_rd_S <= 0;
				next_S_idx_counter <= S_idx_counter;
				en_rd_N <= 1;
				next_rd_addr_N <= rd_addr_N;
				next_monomial <= 1;
				next_sum <= sum;
				next_result <= result;
				next_status <= status;
			end

			STATE_CHECK_N:
			begin
				done_evp <= 0;
				en_rd_data <= 0;
				next_rd_addr_data <= rd_addr_data_updated;
				en_rd_S <= 0;
				next_S_idx_counter <= S_idx_counter;
				en_rd_N <= 0;
				next_rd_addr_N <= rd_addr_N;
				next_monomial <= monomial;
				next_sum <= sum;
				next_result <= result;
				next_status <= status;
			end

			STATE_RD_DATA:
			begin
				done_evp <= 0;
				en_rd_data <= 1;
				x <= ram_out_data; // Capture x value from data token
				next_rd_addr_data <= rd_addr_data_updated + 1;
				en_rd_S <= 1;
				c_i <= ram_out_S;
				next_S_idx_counter <= S_idx_counter + 1;
				en_rd_N <= 0;
				next_rd_addr_N <= rd_addr_N;
				next_monomial <= monomial;
				next_sum <= sum;
				next_result <= result;
				next_status <= status;
			end	

			STATE_COMPUTE_SUM:
			begin
				done_evp <= 0;
				en_rd_data <= 0;
				next_rd_addr_data <= rd_addr_data_updated;
				en_rd_S <= 0;
				next_S_idx_counter <= S_idx_counter;
				en_rd_N <= 0;
				next_rd_addr_N <= rd_addr_N;
				next_monomial <= monomial;
				next_sum <= sum + monomial * c_i;
				next_result <= result;
				next_status <= status;
			end

			STATE_GET_NEXT_COEFF:
			begin
				done_evp <= 0;
				en_rd_data <= 0;
				next_rd_addr_data <= rd_addr_data_updated;
				en_rd_S <= 1;
				c_i <= ram_out_S;
				next_S_idx_counter <= S_idx_counter + 1;
				en_rd_N <= 0;
				next_rd_addr_N <= rd_addr_N;
				next_monomial <= monomial;
				next_sum <= sum;
				next_result <= result;
				next_status <= status;
			end

			STATE_COMPUTE_EXP:
			begin
				done_evp <= 0;
				en_rd_data <= 0;
				next_rd_addr_data <= rd_addr_data_updated;
				en_rd_S <= 0;
				next_S_idx_counter <= S_idx_counter;
				en_rd_N <= 0;
				next_rd_addr_N <= rd_addr_N;
				next_monomial <= monomial * x;
				next_sum <= sum;
				next_result <= result;
				next_status <= status; 
			end

			STATE_OUTPUT:
			begin
				done_evp <= 0;
				en_rd_data <= 0;
				next_rd_addr_data <= rd_addr_data_updated;
				en_rd_S <= 0;
				next_S_idx_counter <= S_idx_counter;
				en_rd_N <= 0;
				next_rd_addr_N <= rd_addr_N;
				next_monomial <= monomial;
				next_sum <= sum;
				next_result <= sum;
				next_status <= 0;
			end

			STATE_ERROR:
			begin
				done_evp <= 0;
				en_rd_data <= 0;
				next_rd_addr_data <= rd_addr_data_updated;
				en_rd_S <= 0;
				next_S_idx_counter <= S_idx_counter;
				en_rd_N <= 0;
				next_rd_addr_N <= rd_addr_N;
				next_monomial <= monomial;
				next_sum <= sum;
				next_result <= 0;
				next_status <= 2'b10;
			end

			STATE_END:
			begin
				done_evp <= 1;
				en_rd_data <= 0;
				next_rd_addr_data <= rd_addr_data_updated;
				en_rd_S <= 0;
				next_S_idx_counter <= S_idx_counter;
				en_rd_N <= 0;
				next_rd_addr_N <= rd_addr_N;
				next_monomial <= monomial;
				next_sum <= sum;
				next_result <= result;
				next_status <= status;
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
