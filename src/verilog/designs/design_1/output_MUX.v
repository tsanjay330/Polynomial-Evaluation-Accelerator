`timescale 1ns/1ps

module output_MUX #(parameter word_size = 16)(
 input [2*word_size - 1 : 0] 	  output_STP,
 input [2*word_size - 1 : 0] 	  output_EVP,
 input [2*word_size - 1 : 0] 	  output_EVB,
 input [7 : 0] 			  instr,
 output reg [2*word_size - 1 : 0] output_token
						  );
   localparam STP = 2'b00, EVP = 2'b01, EVB = 2'b10, RST = 2'b11;

   always@(output_STP, output_EVP, output_EVB)
     begin
	case(instr)
	  STP: output_token <= output_STP;
	  EVP: output_token <= output_EVP;
	  EVB: output_token <= output_EVB;
	endcase // case (instr)
     end
endmodule // output_MUX

   
   
