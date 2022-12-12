`timescale 1ns/1ps

module rd_addr_N_MUX #(parameter n_size = 8)(
     input [log2(n_size) - 1 : 0]      rd_addr_N_EVP,
     input [log2(n_size) - 1 : 0]      rd_addr_N_EVB,
     input [7 : 0] 		       instr,
     output reg [log2(n_size) - 1 : 0] rd_addr_N

					     );
   localparam STP = 2'b00, EVP = 2'b01, EVB = 2'b10, RST = 2'b11;

   always@(rd_addr_N_EVP, rd_addr_N_EVB)
     begin
	case(instr)
	  EVP: rd_addr_N <= rd_addr_N_EVP;
	  EVB: rd_addr_N <= rd_addr_N_EVB;
	endcase // case (instr)
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
   endfunction // log2

endmodule // rd_addr_N_MUX

   
   
