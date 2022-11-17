`timescale 1ns/1ps

module get_command_FSM_3
        (
        input clk, rst,
        input start_get_cmd,
		input [15 : 0] command_in,
		input en_mode_check_err,
		input [3 : 0] N [7 : 0],
		output reg en_rd_cmd,
        output reg done_get_cmd,
        output reg [7 : 0] instr,
        output reg [2 : 0] arg1,
		output reg [4 : 0] arg2,
		output reg [1 : 0] error);

		reg [1 : 0] state, next_state;
		reg [7 : 0] next_instr;
		reg [2 : 0] next_arg1;
		reg [4 : 0] next_arg2;
		reg [1 : 0] next_error;

    localparam STATE_START = 2'b00, STATE_GET_CMD = 2'b01, STATE_CHECK_ERR = 2'b10, STATE_END = 2'b11;

	always @(posedge clk or negedge rst) begin
		if (! rst) begin
			state <= STATE_START;
			instr <= 0;
			arg1 <= 0;
			arg2 <= 0;
			error <= 0;
		end
		else begin
			state <= next_state;
			instr <= next_instr;
			arg1 <= next_arg1;
			arg2 <= next_arg2;
			error <= next_error;
		end
	end

	always @(state, start_get_cmd, en_mode_check_err) begin
		case (state)
			STATE_START:
			begin
				if (start_get_cmd)
					next_state <= STATE_GET_CMD;
				else
					next_state <= STATE_START;
			end

			STATE_GET_CMD:
			begin
				if (en_mode_check_err)
					next_state <= STATE_CHECK_ERR;
				else
					next_state <= STATE_GET_CMD;
			end

			STATE_CHECK_ERR:
				next_state <= STATE_END;

			STATE_END:
				next_state <= STATE_START;

		endcase
	end

	always @(state, start_get_cmd, command_in, instr, arg1, arg2, error) begin
		case (state)
			STATE_START:
			begin
				done_get_cmd <= 0;
				en_rd_cmd <= 0;
				next_instr <= instr;
				next_arg1 <= arg1;
				next_arg2 <= arg2;
				next_error <= error;
			end

			STATE_GET_CMD:
			begin
				done_get_cmd <= 0;
				en_rd_cmd <= 1;
				next_instr <= instr;
				next_arg1 <= arg1;
                next_arg2 <= arg2;
                next_error <= error;
			end

			STATE_CHECK_ERR:
			begin
				case (command_in[15 : 8])
					0:
					begin 
						if (command_in[4 : 0] <= 10) begin
							done_get_cmd <= 0;
							en_rd_cmd <= 0;
							next_instr <= command_in[15 : 8];
							next_arg1 <= command_in[7 : 5];
							next_arg2 <= command_in[4 : 0];
							next_error <= 0;
						end
			 			else begin
							done_get_cmd <= 0;
							en_rd_cmd <= 0;
							next_instr <= instr;
							next_arg1 <= arg1;
							next_arg2 <= arg2;
							next_error <= 2;
						end
					end
		
					1:
					begin
						if (N[command_in[7 : 5]] != 15) begin
							done_get_cmd <= 0;
                        	en_rd_cmd <= 0;
                        	next_instr <= command_in[15 : 8];
                        	next_arg1 <= command_in[7 : 5];
                        	next_arg2 <= 0;
                        	next_error <= 0;
						end
						else begin
							done_get_cmd <= 0;
                            en_rd_cmd <= 0;
                            next_instr <= instr;
                            next_arg1 <= arg1;
                            next_arg2 <= arg2;
                            next_error <= 3;
						end
					end

					2:
					begin
						if (N[command_in[7 : 5]] != 15) begin
                            done_get_cmd <= 0;
                            en_rd_cmd <= 0;
                            next_instr <= command_in[15 : 8];
                            next_arg1 <= command_in[7 : 5];
                            next_arg2 <= command_in[4 : 0];
                            next_error <= 0;
                        end
                        else begin
                            done_get_cmd <= 0;
                            en_rd_cmd <= 0;
                            next_instr <= instr;
                            next_arg1 <= arg1;
                            next_arg2 <= arg2;
                            next_error <= 3;
						end
					end

					3:
					begin
						done_get_cmd <= 0;
						en_rd_cmd <= 0;
						next_instr <= command_in[15 : 8];
						next_arg1 <= 0;
						next_arg2 <= 0;
						next_error <= 0;
					end

					default:
					begin
						done_get_cmd <= 0;
						en_rd_cmd <= 0;
						next_instr <= instr;
						next_arg1 <= arg1;
						next_arg2 <= arg2;
						next_error <= 1;
					end
				
				endcase
			end

			STATE_END:
			begin
				done_get_cmd <= 1;
				en_rd_cmd <= 0;
				next_instr <= instr;
				next_arg1 <= arg1;
				next_arg2 <= arg2;
				next_error <= error;
			end

		endcase
	end
endmodule