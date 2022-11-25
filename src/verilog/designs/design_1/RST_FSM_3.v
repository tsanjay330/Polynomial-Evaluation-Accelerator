`timescale 1ns/1ps

module RST_FSM_3
		(
		input clk, rst,
		input start_rst,
		output reg done_rst);

		reg [1 : 0] state, next_state;

	localparam STATE_START = 2'b00, STATE_RESET = 2'b01;

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
				next_state <= STATE_START;
	
		endcase

	always @(state)
		case (state)
			STATE_START:
				done_rst <= 0;

			STATE_RESET:
				done_rst <= 1;
		
		endcase
endmodule
