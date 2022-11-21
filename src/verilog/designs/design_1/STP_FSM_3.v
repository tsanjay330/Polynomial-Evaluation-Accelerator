`timescale 1ns/1ps

//TODO: could change initial values of result and status

module STP_FSM_3 
		(
		input clk, rst,
		input start_stp,
		input en_mode_wr_coeff,
		input [2 : 0] A,
		input [4 : 0] N,
		input [15 : 0] c,
		output reg en_rd_data,
		output reg done_stp,
		output reg [15 : 0] SA_i,
		output reg [31 : 0] result,
		output reg [4 : 0] status);		// TODO: this bit width depends on number of errors

		reg [1 : 0] state, next_state;
		reg [31 : 0] next_result;
		reg [4 : 0] next_status;
		reg [4 : 0] counter, next_counter;
		reg [15 : 0] next_SA_i;

	localparam STATE_START = 2'b00, STATE_GET_COEFF = 2'b01, 
				STATE_WRITE_COEFF = 2'b10, STATE_END = 2'b11;

	always @(posedge clk, negedge rst)
		if (!rst) begin
			state <= STATE_START;
			result <= 0;
			status <= 5'b11111;
			counter <= 0;
			SA_i <= 0;
		end
		else begin
			state <= next_state;
			result <= next_result;
			status <= next_status;
			counter <= next_counter;
			SA_i <= next_SA_i;
		end

	always @(state, start_stp, en_mode_wr_coeff, counter)
		case (state)
			STATE_START:
				if (start_stp)
					next_state <= STATE_GET_COEFF;
				else
					next_state <= STATE_START;	
			
			STATE_GET_COEFF:
				if (en_mode_wr_coeff)
					next_state <= STATE_WRITE_COEFF;
				else
					next_state <= STATE_GET_COEFF;
			
			STATE_WRITE_COEFF:
				if (counter == N - 1)
					next_state <= STATE_END;
				else
					next_state <= STATE_GET_COEFF;
	
			STATE_END:
				next_state <= STATE_START;
	
		endcase

	always @(state, start_stp, c, result, status)
		case (state)
			STATE_START:
			begin
				done_stp <= 0;
				en_rd_data <= 0;
				next_SA_i <= 0;
				next_result <= result;
				next_status <= status;
				next_counter <= 0;
			end

			STATE_GET_COEFF:
			begin
				done_stp <= 0;
				en_rd_data <= 1;
				next_SA_i <= SA_i;
				next_result <= result;
				next_status <= status;
				next_counter <= 0;
			end

			STATE_WRITE_COEFF:
			begin
				done_stp <= 0;
				en_rd_data <= 0;
				next_SA_i <= c;
				next_result <= result;
				next_status <= status;
				next_counter <= counter + 1;
			end

			STATE_END:
			begin
				done_stp <= 1;
				en_rd_data <= 0;
				next_SA_i <= SA_i;
				next_result <= 1;
				next_status <= 0;
				next_counter <= 0;
			end  
		endcase
endmodule
