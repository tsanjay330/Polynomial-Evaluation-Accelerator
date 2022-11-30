//TODO: add error handling

`timescale 1ns/1ps

module EVB_FSM_3
		#(parameter buffer_size = 1024)(
		input clk, rst,
		input rst_instr,
		input start_evb,
		input [2 : 0] A,
		input [4 : 0] b,
		input [15 : 0] x_b,
		input [15 : 0] c_i,
		input [4 : 0] N,
		input [log2(buffer_size) - 1 : 0] rd_addr_data,
		output reg done_evp,
		output reg done_evb,
		output reg en_rd_data,
		output reg en_rd_S,
		output reg en_rd_N,
		output [log2(buffer_size) - 1 : 0] rd_addr_data_updated,
		output [7 : 0] rd_addr_S,
		output reg [31 : 0] result,
		output reg [31 : 0] status);

		reg [2 : 0] state, next_state;
		reg en_evp;
		reg [31 : 0] next_result, next_status;
		reg [4 : 0] b_counter, next_b_counter;

	localparam STATE_START = 3'b000, STATE_FIRST_EVP0 = 3'b001, 
				STATE_FIRST_EVP1 = 3'b010, STATE_FIRST_EVP2 = 3'b011,
				STATE_EVP0 = 3'b100, STATE_EVP1 = 3'b101, STATE_EVP2 = 3'b110,
				STATE_END = 3'b111;

	EVP_FSM_3 evp
		(.clk(clk), .rst(rst), .start_evp(en_evp), .A(A), .x(x_b), .c_i(c_i),
			.N(N), .rd_addr_data(rd_addr_data), .en_rd_data(en_rd_data), 
			.en_rd_S(en_rd_S), .en_rd_N(en_rd_N), 
			.rd_addr_data_updated(rd_addr_data_updated), .rd_addr_S(rd_addr_S), 
			.done_evp(done_evp), .result(next_result), .status(next_status));

assign rd_addr_data_updated = rd_addr_data + b_counter;

	always @(posedge clk, negedge rst)
		if (! rst || ! rst_instr) begin
			state <= STATE_START;
			b_counter <= 0;
			result <= 0;
			status <= 32'b11111111111111111111111111111111;
		end
		else begin
			state <= next_state;
			b_counter <= next_b_counter;
			result <= next_result;
			status <= next_status;
		end

	always @(state, start_evb, done_evp, b)
		case (state)
			STATE_START:
            begin
                if (start_evb)
                    next_state <= STATE_FIRST_EVP0;
                else
                    next_state <= STATE_START;
            end

            STATE_FIRST_EVP0:
                next_state <= STATE_FIRST_EVP1;

            STATE_FIRST_EVP1:
            begin
                if (done_evp)
                    next_state <= STATE_FIRST_EVP2;
                else
                    next_state <= STATE_FIRST_EVP1;
            end

            STATE_FIRST_EVP2:
            begin
                if (b_counter == b - 1)
                    next_state <= STATE_END;
                else
                    next_state <= STATE_EVP0;
            end

            STATE_EVP0:
                next_state <= STATE_EVP1;

            STATE_EVP1:
            begin
                if (done_evp)
                    next_state <= STATE_EVP2;
                else
                    next_state <= STATE_EVP1;
            end

            STATE_EVP2:
            begin
                if (b_counter == b - 1)
                    next_state <= STATE_END;
                else
					next_state <= STATE_EVP0;
			end

			STATE_END:
				next_state <= STATE_START;

		endcase

	always @(state, b_counter)
		case (state)
			STATE_START:
			begin
				done_evb <= 0;
				en_evp <= 0;
				next_b_counter <= 0;
			end

			STATE_FIRST_EVP0:
			begin
				done_evb <= 0;
				en_evp <= 1;
				next_b_counter <= b_counter;
			end

			STATE_FIRST_EVP1:
			begin
				done_evb <= 0;
				en_evp <= 0;
				next_b_counter <= b_counter;
			end

			STATE_FIRST_EVP2:
			begin
				done_evb <= 0;
				en_evp <= 0;
				next_b_counter <= b_counter + 1;
			end

			STATE_EVP0:
			begin
				done_evb <= 0;
				en_evp <= 1;
				next_b_counter <= b_counter;
			end

			STATE_EVP1:
			begin
				done_evb <= 0;
				en_evp <= 0;
				next_b_counter <= b_counter;
			end

			STATE_EVP2:
			begin
				done_evb <= 0;
				en_evp <= 0;
				next_b_counter <= b_counter + 1;
			end

			STATE_END:
			begin
				done_evb <= 1;
				en_evp <= 0;
				next_b_counter <= b_counter;
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
