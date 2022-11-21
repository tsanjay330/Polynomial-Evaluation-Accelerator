`timescale 1ns/1ps

module mem_controller
#(parameter word_size = 16, buffer_size = 1024, num_vectors = 8, max_degree = 10)(
 input 				   clk, rst,
 input 				   done_get_cmd, // Is this needed?
 input 				   done_STP, // Done/Enable signals from each FSM3 state/mode module
 input 				   done_EVP,
 input 				   done_EVB,
 input 				   done_RST,
 input [log2(buffer_size) - 1 : 0] command_population, // FIFO information - to begin reading tokens
 input [log2(buffer_size) - 1 : 0] data_population,
 input [log2(buffer_size) - 1 : 0] result_free_space, // FIFO information - to stop writing tokens
 input [log2(buffer_size) - 1 : 0] status_free_space,
 input 				   command_FIFO_out_en,
 input 				   data_FIFO_out_en,
 input 				   start_in, // Based on load_loc_mem_FSM_3 of lab 7 - this signal comes from FSM2_firing_state - when data needs to be moved
 input [7 : 0] 			   instr,	// Outputs from Get_command_FSM3 - should these be here or in separate FSM3 modules, or both??
 input [2 : 0] 			   arg1,
 input [4 : 0] 			   arg2,
										  
										  
						

 output 			   command_FIFO_rd_en,
 output 			   data_FIFO_rd_en,
 output 			   result_FIFO_wr_en,
 output 			   status_FIFO_wr_en,
 output 			   c_ram_wr_en,
 output 			   d_ram_wr_en,
 output 			   n_ram_wr_en,
 output 			   s_ram_wr_en						
						);

   // Count variables for how many times a token has been read from FIFO?
   reg [log2(buffer_size) - 1 : 0]	command_count, next_command_count;
   reg [log2(buffer_size) - 1 : 0] 	data_count, next_data_count;
   reg [log2(buffer_size) - 1 : 0] 	count, next_count;

   // Command token signals?
   reg [7 : 0] 				cur_instr;
   reg [2 : 0] 				cur_arg1;
   reg [4 : 0] 				cur_arg2;
   
   

   reg [4 : 0] 				state, next_state;
   

   localparam STATE_START = 4'b0000, STATE_READ_FIFO_EN = 4'b0001, STATE_READ_FIFO = 4'b0010, STATE_READ_RAM_EN, = 4'b0011, STATE_READ_RAM = 4'b0100, STATE_WRITE_RAM_EN = 4'b0101, STATE_WRITE_RAM = 4'b0110, STATE_WRITE_FIFO_EN = 4'b0111, STATE_WRITE_FIFO = 4'b1000, STATE_END = 4'b1001;
   localparam STP = 2'b00, EVP = 2'b01, EVB = 2'b10, RST = 2'b11;

   /*
   
   always@(posedge clk) begin
if(command_population > 0 && command_FIFO_out_en == 1'b1) begin
   command_FIFO_rd_en <= 1'b1;
end
else begin
   command_FIFO_rd_en <= 1'b0;
   
end
      if(data_population > 0 && data_FIFO_out_en == 1'b1) begin
	 data_FIFO_rd_en <= 1'b1;
      end
      else begin
	 data_FIFO_rd_en <= 1'b0;
      end
      
  */
   // Update current state and counter variables on clk edge (or synch reset)
   always@(posedge clk or negedge rst)
   begin
       if(!rst) begin
	  state <= STATE_START;
	  count <= 0;
	  data_count <= 0;
	  command_count <= 0;
	  
       end
       else
	begin
	   state <= next_state;
	   count <= next_count;
	   data_count <= next_data_count;
	   command_count <= next_command_count;
	   
	end

   end

   // 
   always@(state, start_in, done_get_command/*, command_FIFO_out_en, data_FIFO_out_en, counter*/) // what/which counter variable?
   begin
       case(state) // General flow: start -> read_fifo_en -> read_fifo -> depending on context, either read_ram, write_ram, read fifo, or write_fifo
	STATE_START:
	begin
           if(start_in)
             next_state <= STATE_READ_FIFO_EN;
	   else
	     next_state <= STATE_START;
	end
	STATE_READ_FIFO_EN:
        begin
           next_state <= STATE_READ_FIFO;
        end
	STATE_READ_FIFO:
        begin
           if(done_get_command) // If done_get_command is high - make read_ram/write_ram decision based on instr
             next_state <= STATE_READ_FIFO_EN;
	     case(instr)
              	STP:	// STP needs to write data to S and N RAM
	      	begin
		   next_state <= STATE_WRITE_RAM_EN;
		end
	        EVP:	// EVP and EVB read data from RAM
                begin
                   next_state <= STATE_READ_RAM_EN;
                end
	        EVB:
		begin
		   next_state <= STATE_READ_RAM_EN;     
                end
	        RST:	// RST uses no data and immediately writes to result and status FIFOs
                begin
                   next_state <= STATE_WRITE_FIFO_EN;
                end
	        default: next_state <= STATE_START; // If an invalid instr token occurs, go back to start
	     endcase
           else
             next_state <= STATE_START;
        end
	STATE_READ_RAM_EN:
	begin
	   next_state <= STATE_READ_RAM;
	end
	STATE_READ_RAM:
	begin
	    if(done_EVP || done_EVB)
  	      next_state <= STATE_WRITE_FIFO_EN;
	    else
	      next_state <= STATE_READ_RAM; // not too sure about this one - consult w/ group
	end
	STATE_WRITE_RAM_EN:
	begin
	   next_state <= STATE_WRITE_RAM;
	end
	STATE_WRITE_RAM:
	begin
	if(done_STP)
	  next_state <= STATE_START; // not too sure about this one - consult w/ group
	else
  	  next_state <= STATE_WRITE_RAM;
	end
	STATE_WRITE_FIFO_EN:
        begin
           next_state <= STATE_WRITE_FIFO;
        end
        STATE_WRITE_FIFO:
        begin
          next_state <= STATE_START; // not too sure about this one - consult w/ group
        else
          next_state <= STATE_WRITE_RAM;
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
