`timescale 1ns/1ps

module get_command_FSM_3
        #(parameter buffer_size = 1024)(
        input clk, rst,
        input start_get_cmd,
		input [15 : 0] command,
		output reg en_rd_cmd,
        output reg done_get_cmd,
		output reg [log2(buffer_size)-1 : 0] rd_addr_command_updated,
		output reg [log2(buffer_size)-1 : 0] rd_addr_data_updated,
        output reg [7 : 0] instr,
        output reg [2 : 0] arg1,
		output reg [4 : 0] arg2
		);

		reg [1 : 0] state; 
		reg [1 : 0] next_state;
		reg [7 : 0] next_instr;
		reg [2 : 0] next_arg1;
		reg [4 : 0] next_arg2;
		reg [log2(buffer_size)-1 : 0] next_rd_addr_command;

    localparam STATE_START = 2'b00, STATE_GET_CMD = 2'b01, 
				STATE_SPLIT_CMD = 2'b10, STATE_END = 2'b11;

	always @(posedge clk or negedge rst) begin
		if (! rst) begin
			state <= STATE_START;
			rd_addr_command_updated <= 0;
			instr <= 8'b11111111;
			arg1 <= 0;
			arg2 <= 0;
		end
		else begin
			state <= next_state;
			rd_addr_command_updated <= next_rd_addr_command;
			instr <= next_instr;
			arg1 <= next_arg1;
			arg2 <= next_arg2;
		end
	end

	always @(*)//state, start_get_cmd) 
	begin
		case (state)
			STATE_START:
			begin
				if (start_get_cmd)
					next_state <= STATE_GET_CMD;
				else
					next_state <= STATE_START;
			end

			STATE_GET_CMD:
				next_state <= STATE_SPLIT_CMD;

			STATE_SPLIT_CMD:
				next_state <= STATE_END;

			STATE_END:
				next_state <= STATE_START;

		endcase
	end

	always @(*)//state, start_get_cmd,rd_addr_command_updated, command, instr, arg1,arg2) 
	begin
		case (state)
			STATE_START:
			begin
				done_get_cmd <= 0;
				en_rd_cmd <= 0;
				next_rd_addr_command <= rd_addr_command_updated;
				next_instr <= instr;
				next_arg1 <= arg1;
				next_arg2 <= arg2;
			end

			STATE_GET_CMD:
			begin
				done_get_cmd <= 0;
				en_rd_cmd <= 0;
				next_rd_addr_command <= rd_addr_command_updated;
				next_instr <= instr;
				next_arg1 <= arg1;
                next_arg2 <= arg2;
			end

			STATE_SPLIT_CMD:
			begin
				done_get_cmd <= 0;
				en_rd_cmd <= 1;
				next_rd_addr_command <= rd_addr_command_updated + 1;
			    next_instr <= command[15 : 8];
				next_arg1 <= command[7 : 5];
				next_arg2 <= command[4 : 0];
			end	

			STATE_END:
			begin
				done_get_cmd <= 1;
				en_rd_cmd <= 0;
				next_rd_addr_command <= rd_addr_command_updated;
				next_instr <= instr;
				next_arg1 <= arg1;
				next_arg2 <= arg2;
			end

		endcase
	end

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
