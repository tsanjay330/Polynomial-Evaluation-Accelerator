//TODO: add error handling

`timescale 1ns/1ps

module EVP_FSM_3 
		#(parameter buffer_size = 1024)(
		input clk, rst,
		input rst_instr,
		input start_evp,
		input [2 : 0] A,
		input [15 : 0] x,
		input [15 : 0] c_i,
		input [4 : 0] N,
		input [log2(buffer_size) - 1 : 0] rd_addr_data, 
		output reg en_rd_data,
		output reg en_rd_S,
		output reg en_rd_N,
		output reg [log2(buffer_size) - 1 : 0] rd_addr_data_updated,
		output [7 : 0] rd_addr_S,
		output reg done_evp,
		output reg [31 : 0] result,
		output reg [31 : 0] status);

		reg [1 : 0] state, next_state;
		reg [31 : 0] next_result;
		reg [31 : 0] next_status;
		reg [3 : 0] counter, next_counter;
		reg [3 : 0] exp_counter, next_exp_counter;
		reg [31 : 0] monomial, next_monomial;
		reg [31 : 0] sum, next_sum;
		reg [log2(buffer_size) : 0] next_rd_addr_data;

	localparam STATE_START = 3'b000, STATE_COMPUTE0 = 3'b001, 
				STATE_COMPUTE1 = 3'b010, STATE_COMPUTE2 = 3'b011,
				STATE_ERROR = 3'b100, STATE_END = 3'b101;

assign rd_addr_S = A * 11 + counter;

	always @(posedge clk, negedge rst)
		if (! rst || ! rst_instr) begin
			state <= STATE_START;
			monomial <= 1;
			sum <= 0;
			counter <= 0;
			exp_counter <= 0;
			rd_addr_data_updated <= 0;
			result <= 0;
			status <= 32'b11111111111111111111111111111111;
		end
		else begin
			state <= next_state;
			monomial <= next_monomial;
			sum <= next_sum;
			counter <= next_counter;
			exp_counter <= next_exp_counter;
			rd_addr_data_updated <= next_rd_addr_data;
			result <= next_result;
			status <= next_status;
		end

	always @(state, start_evp, counter, N)
		case (state)
			STATE_START:
			begin
				if (start_evp)
					next_state <= STATE_COMPUTE0;
				else
					next_state <= STATE_START;
			end

			STATE_COMPUTE0:
				if (N == 5'b11111)
					next_state <= STATE_ERROR;
				else
					next_state <= STATE_COMPUTE1;

			STATE_COMPUTE1:
			begin
				if (exp_counter == counter)
					next_state <= STATE_COMPUTE2;
				else
					next_state <= STATE_COMPUTE1;
			end

			STATE_COMPUTE2:
			begin
				if (counter == N)
					next_state <= STATE_END;
				else
					next_state <= STATE_COMPUTE1;
			end

			STATE_ERROR:
				next_state <= STATE_END;

			STATE_END:
				next_state <= STATE_START;
	
		endcase

	always @(state, start_evp, c_i, x, monomial, sum, counter, exp_counter, 
				result, status)
		case (state)
			STATE_START:
			begin
				done_evp <= 0;
				en_rd_data <= 0;
				en_rd_S <= 0;
				en_rd_N <= 0;
				next_monomial <= 1;
				next_sum <= 0;
				next_counter <= 0;
				next_exp_counter <= 0;
				next_rd_addr_data <= rd_addr_data;
				next_result <= 0;
				next_status <= 32'b11111111111111111111111111111111;
			end

			STATE_COMPUTE0:
			begin
				done_evp <= 0;
				en_rd_data <= 1;
				en_rd_S <= 1;
				en_rd_N <= 1;
				next_monomial <= monomial;
				next_sum <= sum;
				next_counter <= counter;
				next_exp_counter <= exp_counter;
				next_rd_addr_data <= rd_addr_data_updated + 1;
				next_result <= result;
				next_status <= status;
			end

			STATE_COMPUTE1:
			begin
				done_evp <= 0;
				en_rd_data <= 0;
				en_rd_S <= 0;
				en_rd_N <= 0;
				next_monomial <= monomial * x;
				next_sum <= sum;
				next_counter <= counter;
				next_exp_counter <= exp_counter + 1;
				next_rd_addr_data <= rd_addr_data_updated;
				next_result <= result;
				next_status <= status; 
			end

			STATE_COMPUTE2:
			begin
				done_evp <= 0;
				en_rd_data <= 0;
				en_rd_S <= 1;
				en_rd_N <= 0;
				next_monomial <= 1;
				next_sum <= sum + monomial * c_i;
				next_counter <= counter + 1;
				next_exp_counter <= 0;
				next_rd_addr_data <= rd_addr_data_updated;
				next_result <= result;
				next_status <= status;
			end

			STATE_ERROR:
			begin
				done_evp <= 0;
				en_rd_data <= 0;
				en_rd_S <= 0;
				en_rd_N <= 0;
				next_monomial <= monomial;
				next_sum <= sum;
				next_counter <= counter;
				next_exp_counter <= exp_counter;
				next_rd_addr_data <= rd_addr_data_updated;
				next_result <= 0;
				next_status <= 2'b10;
			end

			STATE_END:
			begin
				done_evp <= 1;
				en_rd_data <= 0;
				en_rd_S <= 0;
				en_rd_N <= 0;
				next_monomial <= monomial;
				next_sum <= sum;
				next_counter <= counter;
				next_exp_counter <= exp_counter;
				next_rd_addr_data <= rd_addr_data_updated;
				next_result <= sum;
				next_status <= 0;
			end
		endcase

	function integer log2;
    input [31 : 0] value;
     integer i;
    begin
          if(value==1)
                log2=1;
          else
              begin
              i = value - 1;
              for (log2 = 0; i > 0; log2 = log2 + 1) begin
                    i = i >> 1;
              end
              end
    end
    endfunction

endmodule	
