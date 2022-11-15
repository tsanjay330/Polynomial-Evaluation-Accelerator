`timescale 1ns / 1ps

module FSM_3 #(parameter word_size = 16, buffer_size = 1024)
   (
    input 			 clk,
    input [word_size - 1 : 0] 	 command_in,
    //input [4:0] 		 b, // Should we use whole command token or just b?
    input [word_size - 1 : 0] 	 data_in,
    input 			 enable, // from enable module - do we need this here?

/***	How will internal signals like N, S, A, and B be used in this module? Where should they be placed in the interface? - discuss w/ team	***/
    
    
    output [2*word_size - 1 : 0] result_out,
    output [2*word_size - 1 : 0] status_out

    output 			 enable_STP; // These enables would activate individual FSM's for each state computation
    output 			 enable_EVP;
    output 			 enable_EVB;
    output 			 enable_RST;
    );

   localparam GET_COMMAND=3'b000, STP=3'b001, EVP=3'b010, EVB=3'b011, OUTPUT=3'b100, RST=3'b101;

   reg 				 en_STP;
   reg 				 en_EVP;
   reg 				 en_EVB;
   reg 				 en_RST;
   

   always@(posedge clk) begin
	case({command_in[word_size - 1],command_in[word_size - 2]})
	  // Do we have a case for GET_COMMAND? this wouldn't make sense
	  STP: begin
	     en_STP <= 1'b1;
             en_EVP <= 1'b0;
             en_EVB <= 1'b0;
             en_RST <= 1'b0;	     
   	  end
	  EVP: begin
             en_STP <= 1'b0;
             en_EVP <= 1'b1;
             en_EVB <= 1'b0;
             en_RST <= 1'b0;	     
	  end
	  EVB: begin
	     en_STP <= 1'b0;
             en_EVP <= 1'b0;
             en_EVB <= 1'b1;
             en_RST <= 1'b0;
	  end
	  RST: begin
	     en_STP <= 1'b0;
             en_EVP <= 1'b0;
             en_EVB <= 1'b0;
             en_RST <= 1'b1;
	  end
	  default: begin
	     en_STP <= 1'b0;
	     en_EVP <= 1'b0;
	     en_EVB <= 1'b0;
	     en_RST <= 1'b0;
	  end
	  
	endcase


   end
endmodule // FSM_3
