/*******************************************************************************This is the module that implements the STP instruction.

--- Inputs ---

clk: clock

rst: reset

start_stp: enable signal for starting an STP instruction

rd_addr_data: address (index) of data RAM

A: first STP argument; index of coefficient vector array we wish to modify

N: second STP argument; length of coefficient vector we wish to modify

next_c: coefficient read in from data RAM

--- Outputs ---

done_stp: signal that indicates STP is finished processing

en_rd_data: enable signal that triggers RAM to read value from data RAM

en_wr_S: enable signal that triggers RAM to write a value to S RAM 

en_wr_N: enable signal that triggers RAM to write a value to N RAM

rd_addr_data_updated: updated address (index) of data RAM

wr_addr_S: address (index) of S RAM we wish to write to
 
wr_addr_N: address (index) of N RAM we wish to write to

c: coefficient to be written to S at address (index) wr_addr_S
 
N_out: The degree of the polynomial to be written to N at wr_addr_N

result: value outputted to the result FIFO

status: value outputted to the status FIFO

--- Local ---

state: current state that the STP module is operating in

next_state: the state that the STP module will operate in next

next_rd_addr_data: temporary register to hold next value of rd_addr_data_updated

next_wr_addr_S: temporary register to hold next value of wr_addr_S

*******************************************************************************/

`timescale 1ns/1ps

module STP_FSM_3 
	#(parameter word_size = 16, buffer_size = 1024, n_size = 8, s_size = 88)(
		input clk, rst,
		input start_stp,
		input [log2(buffer_size)-1 : 0] rd_addr_data,
		input [2 : 0] A,
		input [4 : 0] N,
		input [15 : 0] next_c,
		output reg done_stp,
		output reg en_rd_data,
		output reg en_wr_S,
		output reg en_wr_N,
		output reg [log2(buffer_size)-1 : 0] rd_addr_data_updated,
		output reg [log2(s_size)-1 : 0] wr_addr_S,
		output reg [log2(n_size) - 1 : 0] wr_addr_N,
		output reg [15 : 0] c,
		output reg [4 : 0] N_out,
		output reg [31 : 0] result,
		output reg [31 : 0] status
		);

		reg [2 : 0] state, next_state;
		reg [31 : 0] next_result;
		reg [31 : 0] next_status;
		reg [log2(buffer_size) - 1 : 0] next_rd_addr_data;
		reg [log2(s_size) - 1 : 0] next_wr_addr_S;

	localparam STATE_IDLE = 3'b000, STATE_START = 3'b001, 
				STATE_RD_FIRST_DATA = 3'b010, STATE_WR_COEFF0 = 3'b011, 
				STATE_WR_COEFF1 = 3'b100, STATE_ERROR = 3'b101, 
				STATE_END = 3'b110;

	/* Update state and outputs */
	always @(posedge clk, negedge rst)
		if (! rst) begin
			state <= STATE_IDLE;
			rd_addr_data_updated <= 0;
			wr_addr_S <= 0;
			wr_addr_N <= 0;
			c <= 0;
			N_out <= 5'b1111;
			result <= 0;
			status <= 32'b11111111111111111111111111111111;
		end
		else begin
			state <= next_state;
			rd_addr_data_updated <= next_rd_addr_data;
			wr_addr_S <= next_wr_addr_S;
			wr_addr_N <= A;
			c <= next_c;
			N_out <= N;
			result <= next_result;
			status <= next_status;
		end

	/* Determine the next state of the module */
	always @(state, start_stp, N, wr_addr_S, A)
		case (state)
			STATE_IDLE:
				if(start_stp)
					next_state <= STATE_START;
				else
					next_state <= STATE_IDLE;

			STATE_START:
				if (N > 10)
					next_state <= STATE_ERROR;
				else
					next_state <= STATE_WR_COEFF0;

			STATE_RD_FIRST_DATA:
				next_state <= STATE_WR_COEFF0;
			
			STATE_WR_COEFF0:
				if (wr_addr_S == A * 11 + N)
					next_state <= STATE_END;
				else
					next_state <= STATE_WR_COEFF1;

			STATE_WR_COEFF1:
				if (wr_addr_S == A * 11 + (N-1))
					next_state <= STATE_END;
				else
					next_state <= STATE_WR_COEFF1;
	
			STATE_ERROR:
				next_state <= STATE_END;

			STATE_END:
				next_state <= STATE_IDLE;
	
		endcase

	/* Update outputs and registers according to the state */
	always @(state, wr_addr_S, wr_addr_N, A, N, c, result, status)
		case (state)
			STATE_IDLE:
			begin
                done_stp <= 0;
                en_rd_data <= 0;
                en_wr_S <= 0;
                en_wr_N <= 0;
                next_rd_addr_data <= rd_addr_data_updated;
                next_wr_addr_S <= wr_addr_S;
            end
			
		
			STATE_START:
			begin
				done_stp <= 0;
				en_rd_data <= 0;
				en_wr_S <= 0;
				en_wr_N <= 0;
				next_rd_addr_data <= rd_addr_data;
				next_wr_addr_S <= wr_addr_S;
				next_result <= 0;
				next_status <= 32'b11111111111111111111111111111111;
			end

			STATE_WR_COEFF0:
			begin
				done_stp <= 0;
				en_rd_data <= 1;
				en_wr_S <= 0;
				en_wr_N <= 1;
				next_rd_addr_data <= rd_addr_data_updated + 1;
				next_wr_addr_S <= A * 11;
				next_result <= result;
				next_status <= status;
			end

			STATE_WR_COEFF1:
			begin
				done_stp <= 0;
				en_rd_data <= 1;
				en_wr_S <= 1;
				en_wr_N <= 0;
				next_rd_addr_data <= rd_addr_data_updated + 1;
				next_wr_addr_S <= wr_addr_S + 1;
				next_result <= 1;
				next_status <= 0;
			end

			STATE_ERROR:
			begin
				done_stp <= 0;
				en_rd_data <= 0;
				en_wr_S <= 0;
				en_wr_N <= 0;
				next_rd_addr_data <= rd_addr_data_updated;
				next_wr_addr_S <= wr_addr_S;
				next_result <= 0;
				next_status <= 2'b01;
			end

			STATE_END:
			begin
				done_stp <= 1;
				en_rd_data <= 0;
				en_wr_S <= 1;
				en_wr_N <= 0;
				next_rd_addr_data <= rd_addr_data_updated;
				next_wr_addr_S <= wr_addr_S;
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
