`timescale 1ns/1ps

module EVB_FSM_3
		#(parameter buffer_size = 1024)(
		input 				   clk, rst,
		input 				   start_evb,
		input [2 : 0] 			   A,
		input [4 : 0] 			   b,
		input [15 : 0] 			   x_b,
		input [15 : 0] 			   c_i,
		input [4 : 0] 			   N,
		input [log2(buffer_size) - 1 : 0]  rd_addr_data,
		output reg 			   done_evp,
		output reg 			   done_evb,
		output 				   en_rd_data,
		output 				   en_rd_S,
		output 				   en_rd_N,
		output [log2(buffer_size) - 1 : 0] rd_addr_data_updated,
		//output [6 : 0] rd_addr_S,
		output [2 : 0] 			   rd_addr_S_vec,
		output [log2(11) - 1 : 0] 	   rd_addr_S_coef,
		output [2 : 0] 			   rd_addr_N, 
		output reg [31 : 0] 		   result,
		output reg [31 : 0] 		   status);

		reg [3 : 0] state, next_state;
		reg en_evp;
		wire next_done_evp;
		wire [31 : 0] next_result, next_status;
		reg [4 : 0] b_counter, next_b_counter;

	localparam STATE_START = 4'b0000, STATE_FIRST_EVP_EN_EVP = 4'b0001, 
				STATE_FIRST_EVP_WAIT = 4'b0010, STATE_FIRST_EVP_DONE = 4'b0011,
				STATE_CHECK_B = 4'b0100, STATE_EVP_EN_EVP = 4'b0101,
				STATE_EVP_WAIT = 4'b0110, STATE_EVP_DONE = 4'b0111, 
				STATE_END = 4'b1000;

	EVP_FSM_3 evp
		(.clk(clk), .rst(rst), .start_evp(en_evp), .A(A), .ram_out_data(x_b), .ram_out_S(c_i), 
			.N(N), .rd_addr_data(rd_addr_data), .en_rd_data(en_rd_data), 
			.en_rd_S(en_rd_S), .en_rd_N(en_rd_N), 
			.rd_addr_data_updated(rd_addr_data_updated), .rd_addr_S_vec(rd_addr_S_vec), .rd_addr_S_coef(rd_addr_S_coef), 
			.rd_addr_N(rd_addr_N), .done_evp(next_done_evp), 
			.result(next_result), .status(next_status));

//assign rd_addr_data_updated = rd_addr_data + b_counter;

	always @(posedge clk, negedge rst)
		if (! rst) begin
			state <= STATE_START;
			b_counter <= 0;
			done_evp <= 0;
			result <= 0;
			status <= 32'b11111111111111111111111111111111;
		end
		else begin
			state <= next_state;
			b_counter <= next_b_counter;
			done_evp <= next_done_evp;
			result <= next_result;
			status <= next_status;
		end

	always @(*) // state, start_evb, next_done_evp, b)
		case (state)
			STATE_START:
            begin
                if (start_evb)
                    next_state <= STATE_FIRST_EVP_EN_EVP;
                else
                    next_state <= STATE_START;
            end

            STATE_FIRST_EVP_EN_EVP:
                next_state <= STATE_FIRST_EVP_WAIT;

            STATE_FIRST_EVP_WAIT:
            begin
                if (next_done_evp)
                    next_state <= STATE_FIRST_EVP_DONE;
                else
                    next_state <= STATE_FIRST_EVP_WAIT;
            end

			STATE_FIRST_EVP_DONE:
				next_state <= STATE_CHECK_B;

            STATE_CHECK_B:
            begin
                if (b_counter == b)
                    next_state <= STATE_END;
                else
                    next_state <= STATE_EVP_EN_EVP;
            end

            STATE_EVP_EN_EVP:
                next_state <= STATE_EVP_WAIT;

            STATE_EVP_WAIT:
            begin
                if (next_done_evp)
                    next_state <= STATE_EVP_DONE;
                else
                    next_state <= STATE_EVP_WAIT;
            end

            STATE_EVP_DONE:
            	next_state <= STATE_CHECK_B;

			STATE_END:
				next_state <= STATE_START;

		endcase

	always @(*) // state, b_counter)
		case (state)
			STATE_START:
			begin
				done_evb <= 0;
				en_evp <= 0;
				next_b_counter <= 0;
			end

			STATE_FIRST_EVP_EN_EVP:
			begin
				done_evb <= 0;
				en_evp <= 1;
				next_b_counter <= b_counter;
			end

			STATE_FIRST_EVP_WAIT:
			begin
				done_evb <= 0;
				en_evp <= 0;
				next_b_counter <= b_counter;
			end

			STATE_FIRST_EVP_DONE:
			begin
				done_evb <= 0;
				en_evp <= 0;
				next_b_counter <= b_counter + 1;
			end
	
			STATE_CHECK_B:
			begin
				done_evb <= 0;
				en_evp <= 0;
				next_b_counter <= b_counter;
			end

			STATE_EVP_EN_EVP:
			begin
				done_evb <= 0;
				en_evp <= 1;
				next_b_counter <= b_counter;
			end

			STATE_EVP_WAIT:
			begin
				done_evb <= 0;
				en_evp <= 0;
				next_b_counter <= b_counter;
			end

			STATE_EVP_DONE:
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
