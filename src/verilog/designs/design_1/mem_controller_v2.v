/*
	The memory controller unit is in charge of moving command and data tokens out of the input FIFOs and into the local RAM modules for Command and Data.
 	
 	There will be TWO controllers - one for each FIFO/RAM pair (One in charge of command tokens, the other in charge of data tokens)
 
 	It is also responsible for tracking the current index/write address of the Command and Data RAM (these values should go to 0 on RST instruction). 
 
*/
`timescale 1ns/1ps

module mem_controller
#(parameter word_size = 16, buffer_size = 1024)(
 input 					clk, rst,
 input [log2(buffer_size) - 1 : 0] 	command_population, // FIFO information - to begin reading tokens
 input [log2(buffer_size) - 1 : 0] 	data_population,
 input [log2(buffer_size) - 1 : 0] 	result_free_space, // FIFO information - to stop writing tokens
 input [log2(buffer_size) - 1 : 0] 	status_free_space,
 input 					command_FIFO_out_en,
 input 					data_FIFO_out_en,
 input [log2(word_size) - 1 : 0]	command_token,
 input [log2(word_size) - 1 : 0] 	data_token,
 input 					start_in, // Based on load_loc_mem_FSM_3 of lab 7 - this signal comes from FSM2_firing_state - when data needs to be moved
							  		  
 output reg 				command_FIFO_rd_en,
 output reg 				data_FIFO_rd_en,
 output reg 				c_ram_wr_en,
 output reg 				d_ram_wr_en,
 output reg [log2(buffer_size) - 1 : 0] c_ram_wr_addr,
 output reg [log2(buffer_size) - 1 : 0] d_ram_wr_addr,
 output reg [log2(word_size) - 1 : 0] command_out,
 output reg [log2(word_size) - 1 : 0] data_out
);

   // Count variables for how many times a token has been written to C & D RAM
   // act as the wr address to the RAM modules
   reg [log2(buffer_size) - 1 : 0]      next_c_ram_wr_addr;
   reg [log2(buffer_size) - 1 : 0]      next_d_ram_wr_addr;
   reg [log2(buffer_size) - 1 : 0] 	c_count, next_c_count, d_count, next_d_count;
   // This running count will be used to determine if a token needs to be read or not
   
   // 3 bit state register
   reg [2 : 0] 				state, next_state;
   
   // All possible states
   localparam STATE_START = 3'b000, STATE_READ_FIFO_EN = 3'b001, STATE_READ_FIFO = 3'b010, STATE_WR_RAM_EN = 3'b011, STATE_WR_RAM = 3'b100, STATE_END = 3'b101;

   // Don't think I can do this - if there's an error around line 44 - you know why
   c_count = 0;
   next_c_count = 0;
   d_count = 0;
   next_d_count = 0;
   
   /***	Update current state and counter outputs on clk edge (or synch reset)	***/
   always@(posedge clk)
   begin
       if(rst) begin
	  state <= STATE_START;
	  c_ram_wr_addr <= 0;
	  d_ram_wr_addr <= 0;
	  c_count <= command_population;
	  d_count <= data_population;
	  
       end
       else
	begin
	   state <= next_state;
	   c_ram_wr_addr <= next_c_ram_wr_addr;
	   d_ram_wr_addr <= next_d_ram_wr_addr;
	   c_count <= next_c_count;
	   d_count <= next_d_count;
	end

   end

   /***	STATE TRANSITION BLOCK	***/
   always@(state, start_in/*, command_FIFO_out_en, data_FIFO_out_en*/)
   begin
       case(state)	// General flow: start -> read_fifo_en -> read_fifo ->
	 		// write_ram_en -> write_ram -> end, retrun to start on end/rst
	STATE_START:
	begin
           if(start_in)
             next_state <= STATE_READ_FIFO_EN;
	   else
	     next_state <= STATE_START;
	end
	STATE_READ_FIFO_EN:
        begin
	   if(rst)
	     next_state <= STATE_START;
	   else           
	     next_state <= STATE_READ_FIFO;
        end
	STATE_READ_FIFO:
        begin
	   if(rst)
             next_state <= STATE_START;
           else  
	     next_state <= STATE_WR_RAM_EN;
	end
	STATE_WR_RAM_EN:
        begin
	   if(rst)
             next_state <= STATE_START;
           else  
             next_state <= STATE_WR_RAM;
        end
	STATE_WR_RAM:
        begin
	   if(rst)
             next_state <= STATE_START;
           else  
             next_state <= STATE_END;
        end
	default:
	  next_state <= STATE_START;
       endcase
   end

   /***	OUTPUT SIGNAL BLOCK and Update Counters	***/
always@(state, start_in)
begin
case(state)
   STATE_START:
   begin
      command_FIFO_rd_en <= 1'b0;
      data_FIFO_rd_en <= 1'b0;
      c_ram_wr_en <= 1'b0;
      d_ram_wr_en <= 1'b0;
      c_ram_wr_addr <= 0;
      d_ram_wr_addr <= 0;
      command_out <= 0;
      data_out <= 0;
      next_c_count <= c_count;
      next_d_count <= d_count;
   end // case: START
   STATE_READ_FIFO_EN:
   begin
      command_FIFO_rd_en <= 1'b0;
      data_FIFO_rd_en <= 1'b0;
      c_ram_wr_en <= 1'b0;
      d_ram_wr_en <= 1'b0;
      c_ram_wr_addr <= 0;
      d_ram_wr_addr <= 0;
      command_out <= 0;
      data_out <= 0;
   end // case: STATE_READ_FIFO_EN
   STATE_READ_FIFO:
   begin

   end // case: STATE_READ_FIFO
   STATE_WRITE_RAM_EN:
   begin

   end
   STATE_WRITE_RAM:
   begin
	
   end // case: STATE_WRITE_RAM
   STATE_END:
   begin

   end
   default:
   begin

   end
endcase // case (state)
   
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
