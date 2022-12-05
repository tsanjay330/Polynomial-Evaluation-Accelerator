`timescale 1ns/1ps

module RST_FSM_3
		(
		input clk,
		input start_rst,
		input [log2(buffer_size)-1 : 0] rd_addr_command,
		input [log2(buffer_size)-1 : 0] rd_addr_data, 
		output reg rst,
		output reg [log2(buffer_size)-1 : 0] rd_addr_command_updated,
		output reg [log2(buffer_size)-1 : 0] rd_addr_data_updated,
		output reg done_rst);

		reg [1 : 0] state, next_state;

	localparam STATE_START = 2'b00, STATE_RESET = 2'b01, STATE_END = 2'b10;

	always @(posedge clk) begin
		state <= next_state;
	end

	always @(state, start_rst)
		case (state)
			STATE_START:
			begin
				if (start_rst)
					next_state <= STATE_RESET;
				else
					next_state <= STATE_START;
			end

			STATE_RESET:
				next_state <= STATE_END;

			STATE_END:
				next_state <= STATE_START;
	
		endcase

	always @(state)
		case (state)
			STATE_START:
			begin
				rst <= 1;
				rd_addr_command_updated <= rd_addr_command;
				rd_addr_data_updated <= rd_addr_data;
				done_rst <= 0;
			end

			STATE_RESET:
			begin
				rst <= 0;
				rd_addr_command_updated <= 0;
				rd_addr_data_updated <= 0;
				done_rst <= 0;
			end

			STATE_END:
			begin
				rst <= 1;
				rd_addr_command_updated <= rd_addr_command;
				rd_addr_data_updated <= rd_addr_data;
				done_rst <= 1;
			end
		
		endcase
endmodule
