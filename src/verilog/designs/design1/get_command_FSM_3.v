`timescale 1ns/1ps

module get_command_FSM_3
        (
        input clk, rst,
        input start_get_cmd,
		input [15 : 0] commandstr,
		input en_proc_cmd,
		output reg en_rd_cmd,
        output reg done_out,
        output reg [7 : 0] command,
        output reg [2 : 0] arg1,
		output reg [4 : 0] arg2,
		output reg [3 : 0] error); //TODO: correct bit width of error

		reg [15 : 0] commandstr;
		reg [7 : 0] next_command, next_command_tmp;
		reg [2 : 0] next_arg1, next_arg1_tmp;
		reg [4 : 0] next_arg2, next_arg2_tmp;

    localparam STATE_START = 3'b000, STATE_GET_CMD = 3'b001, STATE_PROC_CMD = 3'b010, STATE_CHECK_ERR = 3'b011, STATE_END = 3'b100;

	always @(posedge clk or negedge reset)
		if (! rst)
			state <= STATE_START;
			command <= 0;
			arg1 <= 0;
			arg2 <= 0;
		else
			state <= next_state;
			command <= next_command;
			arg1 <= next_arg1;
			arg2 <= next_arg2;

	always @(state, start_get_cmd, en_proc_cmd) begin
		case (state)
			STATE_START:
				if (start_get_cmd)
					next_state <= STATE_GET_CMD;
				else
					next_state <= STATE_START;

			STATE_GET_CMD:
				if (en_proc_cmd)
					next_state <= STATE_PROC_CMD;
				else
					next_state <= STATE_GET_CMD;

			STATE_PROC_CMD:
				next_state <= STATE_CHECK_ERR;

			STATE_CHECK_ERR:
				next_state <= STATE_END;

			STATE_END:
				next_state <= STATE_START;

		endcase
	end

	always @(state, start_get_cmd, commandstr, command, arg1, arg2, error) begin
		case (state)
			STATE_START:
				done_get_cmd <= 0;
				en_rd_cmd <= 0;
				next_command <= command;
				next_arg1 <= arg1;
				next_arg2 <= arg2;
				next_error <= error;
				next_command_tmp <= 0;
				next_arg1_tmp <= 0;
				next_arg2_tmp <= 0;

			STATE_GET_CMD:
				done_get_cmd <= 0;
				en_rd_cmd <= 1;
				next_command <= command;
				next_arg1 <= arg1;
                next_arg2 <= arg2;
                next_error <= error;
                next_command_tmp <= 0;
                next_arg1_tmp <= 0;
                next_arg2_tmp <= 0;

			STATE_PROC_CMD:
				done_get_cmd <= 0;
				en_rd_cmd <= 0;
				next_command <= command;
                next_arg1 <= arg1;
                next_arg2 <= arg2;
                next_error <= error;
                next_command_tmp <= commandstr[15 : 8];
                next_arg1_tmp <= commandstr[7 : 5];
                next_arg2_tmp <= commandstr[4 : 0];

			STATE_CHECK_ERR:
				done_get_cmd <= 0;
				en_rd_cmd <= 0;

				case (next_command_tmp)
					0: 
						if (next_arg2_tmp <= 10) begin
							next_command <= next_command_tmp;
							next_arg1 <= next_arg1_tmp;
							next_arg2 <= next_arg2_tmp;
							next_error <= 2;
						end
			 			else begin
							next_command <= command;
							next_arg1 <= arg1;
							next_arg2 <= arg2;
							next_error <= 1;
						end
		
					1:
						next_command <= next_command_tmp;
                        next_arg1 <= next_arg1_tmp;
                        next_arg2 <= 1;
                        next_error <= 0;

					2:
						next_command <= next_command_tmp;
						next_arg1 <= next_arg1_tmp;
						next_arg2 <= next_arg2_tmp;
						next_error <= 0;

					3:
						next_command <= next_command_tmp;
						next_arg1 <= 0;
						next_arg2 <= 0;
						next_error <= 0;

					default:
						next_command <= command;
						next_arg1 <= arg1;
						next_arg2 <= arg2;
						next_error <= 1;
				
				endcase

			// kinda stuck on this next vs next tmp thing	
