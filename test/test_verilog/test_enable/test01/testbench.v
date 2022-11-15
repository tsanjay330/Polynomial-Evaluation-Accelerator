`timescale 1ns / 1ps
module testbench();
    parameter word_size = 16, buffer_size = 1024;
    parameter GET_COMMAND = 3'b000, STP = 3'b001, EVP = 3'b010, EVB = 3'b011, OUTPUT = 3'b100, RST = 3'b101;

// Inputs to enable module (all wires here are outputs from FIFO modules)
   reg clk;
   reg rst;
   reg [log2(buffer_size) - 1 : 0] command_pop;
   reg [log2(buffer_size) - 1 : 0] data_pop;
   wire [log2(buffer_size) - 1 : 0] result_free_space;
   wire [log2(buffer_size) - 1 : 0] status_free_space;
   reg [2 : 0] mode;//3-bit operations mode
   reg [4 : 0] b;
   reg [3 : 0] N;
/*   reg [word_size - 1 : 0] command_in;
   reg [word_size - 1 : 0] data_in;*/
// Outputs to enable module
   wire enable;
   wire 
// Inputs to FIFO modules
   reg result_wr_en, result_rd_en;
   reg [word_size - 1 : 0] result_data_in;
   
   
   integer i;
   
/******************************************************
 Instantiate 4 FIFOs and the enable module for the PEA
 *******************************************************/

   // Instantiate the enable module
   PEA_enable #(word_size, buffer_size) enable(rst, control_pop, data_pop, result_free_space, status_free_space, mode, control_in, enable);

   // Instantiate the 4 FIFOs: control, data, result, and status
   
   
   
initial begin
   // Clock alternates with a period of 2 time units
   for(i = 0; i < 200; i = i + 1) begin
      #1 clk <= 1;
      #1 clk <= 0;
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
