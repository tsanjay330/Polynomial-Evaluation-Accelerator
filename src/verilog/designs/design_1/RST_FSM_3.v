/*******************************************************************************This is the module that implements the STP instruction.

--- Inputs ---

clk: clock

rst: reset

--- Outputs ---

rst_instr: signal that triggers a reset

--- Local ---

state: current state that the RST module is operating in

next_state: the state that the RST module will operate in next

*******************************************************************************/

`timescale 1ns/1ps

module RST_FSM_3
		(
		input clk, rst,
		input start_rst,
		output reg rst_instr);

		reg [1 : 0] state, next_state;

	localparam STATE_START = 2'b00, STATE_RESET = 2'b01;

	/* Update state */
	always @(posedge clk, negedge rst) begin
		if (! rst)
			state <= STATE_START;
		else
			state <= next_state;
	end

	/* Determine the next state of the module */
	always @(state, start_rst)
		case (state)
			STATE_START:
				if (start_rst)
					next_state <= STATE_RESET;
				else
					next_state <= STATE_START;

			STATE_RESET:
				next_state <= STATE_START;

		endcase

	/* Update the output according to the state */
	always @(state)
		case (state)
			STATE_START:
				rst_instr <= 1;

			STATE_RESET:
				rst_instr <= 0;

		endcase

endmodule
