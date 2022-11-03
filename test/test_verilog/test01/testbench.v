`timescale 1ns / 1ps
module testbench();
   localparam buffer_size = 1024;
   localparam word_size = 16;

// Inputs to enable module
   reg rst;
   reg [log2(buffer_size) - 1 : 0] control_pop;
   reg [log2(buffer_size) - 1 : 0] data_pop;
   reg [log2(buffer_size) - 1 : 0] result_free_space;
   reg [log2(buffer_size) - 1 : 0] status_free_space;
   reg [1 : 0] 			   mode;
   reg [word_size - 1 : 0] 	   control_in;
// Outputs to enable module
   wire 			   enable;
   



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
