`timescale 1ns/1ps

module RST_FSM_3
		(
		input clk, rst,
		input start_rst,
		output reg rst_instr);
		//output reg done_rst);

		reg [1 : 0] state, next_state;

	localparam STATE_START = 2'b00, STATE_RESET = 2'b01; //, STATE_END = 2'b10;

	always @(posedge clk, negedge rst) begin
		if (! rst)
			state <= STATE_START;
		else
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

			/*STATE_END:
				next_state <= STATE_START;*/
	
		endcase

	always @(state)
		case (state)
			STATE_START:
			begin
				rst_instr <= 1;
				//done_rst <= 0;
			end

			STATE_RESET:
			begin
				rst_instr <= 0;
				//done_rst <= 0;
			end

			/*STATE_END:
			begin
				rst <= 1;
				done_rst <= 1;
			end*/
		
		endcase
endmodule
