/*
	The memory controller unit is in charge of moving command and data tokens out of the input FIFOs and into the local RAM modules for Command and Data.
 
	This module assumes the FIFOs are the regular fifo.v modules, NOT the edited ones.
 	
 	There will be TWO instantiated controllers - one for each FIFO/RAM pair (One in charge of command tokens, the other in charge of data tokens)
 
 	It is also responsible for tracking the current index/write address of the Command and Data RAM (these values should go to 0 on RST instruction). 
 
*/
`timescale 1ns/1ps

module mem_controller
#(parameter word_size = 16, buffer_size = 1024)(
	input clk, rst, rst_instr,
	input [log2(buffer_size) - 1 : 0] FIFO_population, // FIFO information - to begin reading tokens
	input [word_size - 1 : 0] input_token,
	output reg 	FIFO_rd_en,
	output reg 	ram_wr_en,
	output reg [log2(buffer_size) - 1 : 0] ram_wr_addr,
	output reg [word_size - 1 : 0] output_token
);
    // Count variables for how many times a token has been written to C & D RAM
    // act as the wr address to the RAM modules
    reg [log2(buffer_size) - 1 : 0] next_ram_wr_addr;
	reg [log2(buffer_size) - 1 : 0] temp_ram_wr_addr;



    // 3 bit state register
    reg [2 : 0] 				state, next_state;
	   
   // All possible states
   localparam STATE_START = 3'b000, STATE_READ_FIFO_EN = 3'b001, STATE_READ_FIFO = 3'b010, STATE_WR_RAM = 3'b011, STATE_END = 3'b100;

   /***	Update current state and counter outputs on clk edge (or synch reset)	***/
	always@(posedge clk)
	begin
    	if(!rst || !rst_instr) begin
			state <= STATE_START;
	 		temp_ram_wr_addr <= 0;
       	end
    else
	begin
		state <= next_state;
		temp_ram_wr_addr <= next_ram_wr_addr;
	end

    end

   /***	STATE TRANSITION BLOCK	***/
	always@(state, FIFO_population, rst)
	begin
    	case(state)	// General flow: start -> read_fifo_en -> read_fifo ->
	 		// write_ram -> end, retrun to start on end or rst
		STATE_START:
		begin
        	if(FIFO_population >= 1)
            	next_state <= STATE_READ_FIFO_EN;
	  		else
	    		next_state <= STATE_START;
		end
		STATE_READ_FIFO_EN:
        begin
	    	if(!rst)
	     		next_state <= STATE_START;
	   		else           
	     		next_state <= STATE_READ_FIFO;
        end
		STATE_READ_FIFO:
        begin
	   		if(!rst)
            	next_state <= STATE_START;
            else  
	     		next_state <= STATE_WR_RAM;
		end
	    STATE_WR_RAM:
        begin
	   if(!rst)
             next_state <= STATE_START;
           else  
             next_state <= STATE_END;
        end
	default:
	  next_state <= STATE_START;
       endcase
   end

   /***	OUTPUT SIGNAL BLOCK and Update Counters	***/
	always@(state)
	begin
		case(state)
   		STATE_START:
   		begin
      		FIFO_rd_en <= 0;
      		ram_wr_en <= 0;
      		next_ram_wr_addr <= temp_ram_wr_addr;
			ram_wr_addr <= temp_ram_wr_addr;
      		output_token <= 0;
   		end 
   		STATE_READ_FIFO_EN:
   		begin
      		FIFO_rd_en <= 1;
      		ram_wr_en <= 0;
      		next_ram_wr_addr <= temp_ram_wr_addr;
			ram_wr_addr <= temp_ram_wr_addr;
	  		output_token <= input_token;
   		end 
   		STATE_READ_FIFO:
   		begin
      		FIFO_rd_en <= 0;
      		ram_wr_en <= 0;
      		next_ram_wr_addr <= temp_ram_wr_addr;
			ram_wr_addr <= temp_ram_wr_addr;
      		output_token <= input_token;
		end
   		STATE_WR_RAM:
   		begin
      		FIFO_rd_en <= 0;
      		ram_wr_en <= 1;
      		next_ram_wr_addr <= temp_ram_wr_addr + 1; // Increment RAM Address
			ram_wr_addr <= temp_ram_wr_addr;
  			output_token <= input_token;
   		end
   		STATE_END:
   		begin 
      		FIFO_rd_en <= 0;
      		ram_wr_en <= 0;
      		next_ram_wr_addr <= temp_ram_wr_addr;
			ram_wr_addr <= temp_ram_wr_addr;
      		output_token <= input_token;   
		end
   		default:
  		begin
      		FIFO_rd_en <= 0;
      		ram_wr_en <= 0;
      		next_ram_wr_addr <= ram_wr_addr;
      		output_token <= input_token;
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
