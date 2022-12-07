/*******************************************************************************This is the module that implements the STP instruction.

--- Inputs ---

clk: clock

rst: reset

start_stp: enable signal for starting an STP instruction

read_addr_data: address (index) of data RAM

A: first STP argument; index of coefficient vector array we wish to modify

N: second STP argument; length of coefficient vector we wish to modify

next_c: coefficient read in from data RAM

--- Outputs ---

done_stp: signal that indicates STP is finished processing

en_rd_data: enable signal that triggers RAM to read value from data RAM

en_wr_S: enable signal that triggers RAM to write a value to S RAM 

rd_addr_data_updated: updated address (index) of data RAM

wr_addr_S: address (index) of S RAM we wish to write to
 
wr_addr_N: address (index) of N RAM we wish to write to

c: coefficient to be written to S at wr_addr_S
 
N_out: The degree of the polynomial to be written to N at wr_addr_N

result: value outputted to the result FIFO

status: value outputted to the status FIFO

fifo_wr_en_r: Write enable signal to the result output FIFO

fifo_wr_en_s: Write enable signal to the status output FIFO 

*******************************************************************************/

`timescale 1ns/1ps

//TODO: could change initial values of result and status
// testing testing 123
module STP_FSM_3 
		#(parameter word_size = 16, buffer_size = 1024, n_size = 8, s_size = 88)(
		input 				     clk, rst,
		input 				     start_stp,
//		input [log2(buffer_size)-1 : 0]      rd_addr_data,
		input [2 : 0] 			     A,
		input [4 : 0] 			     N,
		input [15 : 0] 			     next_c,
		output reg 			     done_stp,
		output reg 			     en_rd_data,
		output reg 			     en_wr_S,
		output reg 			     en_wr_N,
		output reg [log2(buffer_size)-1 : 0] rd_addr_data_updated,
		output reg [log2(s_size)-1 : 0]      wr_addr_S,
		output reg [log2(n_size) - 1 : 0]    wr_addr_N,
		output reg [15 : 0] 		     c,
		output reg [4 : 0] 		     N_out,
		output reg [31 : 0] 		     result,
		output reg [31 : 0] 		     status
		);

		reg [2 : 0] state, next_state;
		reg [31 : 0] next_result;
		reg [31 : 0] next_status;
		reg [log2(buffer_size) - 1 : 0] next_rd_addr_data;
		reg [log2(s_size) - 1 : 0] next_wr_addr_S;
   		//reg [log2(n_size) - 1] 		      next_wr_addr_N;

	localparam STATE_START = 3'b000, STATE_RD_FIRST_DATA = 3'b001, 
				STATE_WR_COEFF0 = 3'b010, STATE_WR_COEFF1 = 3'b011,
				STATE_ERROR = 3'b100, STATE_END = 3'b101;

   /*	Update State and Outputs Block	*/
	always @(posedge clk, negedge rst)
		if (! rst) begin
			state <= STATE_START;
		   	rd_addr_data_updated <= 0;
			wr_addr_S <= 0;
			wr_addr_N <= 0;
			c <= 0;
			N_out <= 5'b1111; // is this necessary?
			result <= 0;
			status <= 32'b11111111111111111111111111111111;
		end
		else begin
			state <= next_state;
			rd_addr_data_updated <= next_rd_addr_data;
			wr_addr_S <= next_wr_addr_S;
			wr_addr_N <= A; // Is this correct?
			c <= next_c;
			N_out <= N;
			result <= next_result;
			status <= next_status;
		end

	always @(state, start_stp, N, wr_addr_S, A)
		case (state)
			STATE_START:
				if (start_stp)
					next_state <= STATE_WR_COEFF0;
				else
					next_state <= STATE_START;	
			
			STATE_RD_FIRST_DATA:
				if (N > 10)
					next_state <= STATE_ERROR;
				else
					next_state <= STATE_WR_COEFF0;
			
			STATE_WR_COEFF0:
				if (wr_addr_S == A * 11 + (N))
					next_state <= STATE_END;
				else
					next_state <= STATE_WR_COEFF1;

			STATE_WR_COEFF1:
				if (wr_addr_S == A * 11 + (N-2))//Minus two becasue one cycle is used to get a value in STATE_WR_COEFF0 and it should already be (N-1) for the if statement since wr_addr_S starts at 0.
					next_state <= STATE_END;
				else
					next_state <= STATE_WR_COEFF1;
	
			STATE_ERROR:
				next_state <= STATE_END;

			STATE_END:
				next_state <= STATE_START;
	
		endcase

	always @(state, wr_addr_S, wr_addr_N, A, N, c, result, status)
		case (state)
			STATE_START:
			begin
				done_stp <= 0;
				en_rd_data <= 0;
				en_wr_S <= 0;
				en_wr_N <= 0;
				next_rd_addr_data <= rd_addr_data_updated;
				next_wr_addr_S <= wr_addr_S;
				wr_addr_N <= wr_addr_N;
				next_result <= 0;
				next_status <= 32'b11111111111111111111111111111111;
			end

			STATE_WR_COEFF0:
			begin
				done_stp <= 0;
				en_rd_data <= 1;
				en_wr_S <= 0;
				en_wr_N <= 1;
				next_rd_addr_data <= rd_addr_data_updated + 1;
				next_wr_addr_S <= A * 11;
				wr_addr_N <= A;
				next_result <= result;
				next_status <= status;
			end

			STATE_WR_COEFF1:
			begin
				done_stp <= 0;
				en_rd_data <= 1;
				en_wr_S <= 1;
				en_wr_N <= 0;
				next_rd_addr_data <= rd_addr_data_updated + 1;
				next_wr_addr_S <= wr_addr_S + 1;
				wr_addr_N <= wr_addr_N;
				next_result <= 1;
				next_status <= 0;
			end

			STATE_ERROR:
			begin
				done_stp <= 0;
				en_rd_data <= 0;
				en_wr_S <= 0;
				en_wr_N <= 0;
				next_rd_addr_data <= rd_addr_data_updated;
				next_wr_addr_S <= wr_addr_S;
				wr_addr_N <= wr_addr_N;
				next_result <= 0;
				next_status <= 2'b10;
			end

			STATE_END:
			begin
				done_stp <= 1;
				en_rd_data <= 0;
				en_wr_S <= 1;
				en_wr_N <= 0;
				next_rd_addr_data <= rd_addr_data_updated; // Noticed that rd_addr_data was 1 too high at end of STP
				next_wr_addr_S <= wr_addr_S;
				next_result <= result;
				next_status <= status;
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
