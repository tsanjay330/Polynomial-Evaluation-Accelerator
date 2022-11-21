`timescale 1ns/1ps

module mem_controller
#(parameter word_size = 16, buffer_size = 1024, num_vectors = 8, max_degree = 10)(
 input 				   clk, rst,
 input 				   done_get_cmd, // Is this needed?
 input 				   done_STP, // Done/Enable signals from each FSM3 state/mode module
 input 				   done_EVP,
 input 				   done_EVB,
 input 				   done_RST,
 input [log2(buffer_size) - 1 : 0] command_population,
 input [log2(buffer_size) - 1 : 0] data_population,
 input [log2(buffer_size) - 1 : 0] result_free_space,
 input [log2(buffer_size) - 1 : 0] status_free_space,
 input 				   command_out_en,
 input 				   data_out_en,
										  
						

 output 			   command_FIFO_rd_en,
 output 			   data_FIFO_rd_en,
 output 			   result_FIFO_wr_en,
 output 			   status_FIFO_wr_en,
 output 			   c_ram_wr_en,
 output 			   d_ram_wr_en,
 output 			   n_ram_wr_en,
 output 			   s_ram_wr_en						
						);

   reg [log2(buffer_size) - 1 : 0]	command_count;
   reg [log2(buffer_size) - 1 : 0] 	data_count;

   
   always@(posedge clk) begin
if(command_population > 0 && command_out_en == 1'b1) begin
   command_FIFO_rd_en <= 1'b1;
end
else begin
   command_FIFO_rd_en <= 1'b0;
   
end
      if(data_population > 0 && data_out_en == 1'b1) begin
	 data_FIFO_rd_en <= 1'b1;
      end
      else begin
	 data_FIFO_rd_en <= 1'b0;
      end
      
  
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
