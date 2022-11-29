`timescale 1ns / 1ps
module testbench();
   parameter word_size = 16, buffer_size = 1024;
   parameter NUM_C_TOKENS = 3, NUM_D_TOKENS = 6;

// External Inputs
   reg clk;
   reg rst;
// Inputs to FIFOs
   reg fifo_wr_en_c, fifo_wr_en_d;
   wire fifo_rd_en_c, fifo_rd_en_d; // Output from mem. controller to FIFOs
   reg [word_size - 1 : 0] fifo_in_c, fifo_in_d;

   // FIFO Outputs
   wire [log2(buffer_size) - 1 : 0] fifo_pop_c, fifo_pop_d;
   wire [log2(buffer_size) - 1 : 0] fifo_free_space_c, fifo_free_space_d;
   wire [word_size - 1 : 0] 	    fifo_out_c, fifo_out_d;
   

   // Inputs to memory controller
   reg 				    start_in_c, start_in_d;

   // Memory Controller Outputs
   wire 			    ram_c_wr_en, ram_d_wr_en;
   wire [log2(buffer_size) - 1 : 0] ram_c_wr_addr, ram_d_wr_addr;
   wire [log2(word_size) - 1 : 0]   mc_output_c, mc_output_d;
   
   // Additional Signals   
   integer i;
   reg [word_size - 1 : 0] input_mem_C [0 : NUM_C_TOKENS - 1];
   reg [word_size - 1 : 0] input_mem_D [0 : NUM_D_TOKENS - 1];
   
/******************************************************
 Instantiate input FIFOs
 *******************************************************/

  fifo #(buffer_size, word_size) fifo_in_command(clk, rst, fifo_wr_en_c, fifo_rd_en_c, fifo_in_c, fifo_pop_c, fifo_free_space_c, fifo_out_c);

  fifo #(buffer_size, word_size) fifo_in_data(clk, rst, fifo_wr_en_d, fifo_rd_en_d, fifo_in_d, fifo_pop_d, fifo_free_space_d, fifo_out_d);
 
  integer descr, descr_c, descr_d; 
   
   
initial begin
   // Clock alternates with a period of 2 time units
   for(i = 0; i < 200; i = i + 1) begin
      #1 clk <= 1;
      #1 clk <= 0;
   end

end

initial 
  begin
     //descr_c = $fopen("c_out.txt");
     //descr_d = $fopen("d_out.txt");
     descr = $fopen("out.txt");
     
     $readmemh("input_c.txt", input_mem_C);
     $readmemh("input_d.txt", input_mem_D);

     $fdisplay(descr, "Setting up input FIFOs");
     for(i = 0; i < NUM_C_TOKENS; i = i + 1)
       begin
	  #2;
	  fifo_in_c <= input_mem_C[i];
	  #2;
	  fifo_wr_en_c <= 1;
	  $fdisplay(descr, "input_c[%d] = %d", i, fifo_in_c);
	  #2;
	  fifo_wr_en_c <= 0;
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
