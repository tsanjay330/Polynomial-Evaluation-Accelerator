`timescale 1ns/1ps

module rd_addr_S_MUX #(parameter s_size = 88)(

 input [log2(s_size) - 1 : 0]  rd_addr_S_EVP,
 input [log2(s_size) - 1 : 0]  rd_addr_S_EVB,
 input [7 : 0] 		       instr,
 output [log2(s_size) - 1 : 0] rd_addr_S
						);
   localparam STP = 2'b00, EVP = 2'b01, EVB = 2'b10, RST = 2'b11;

   assign rd_addr_S = (instr - 1) ? rd_addr_S_EVP : rd_addr_S_EVB;
   
   
   /*always@(rd_addr_S_EVP, rd_addr_S_EVB)
     begin
case(instr)
EVP =
endcase
  
     end*/ // UNMATCHED !!

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
   endfunction // log2
endmodule

   
