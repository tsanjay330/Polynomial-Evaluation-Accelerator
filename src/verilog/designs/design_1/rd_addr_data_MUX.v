// This module sets rd addr data to 0 on active reset.
// Otherwise, set it to the active FSM 3 module's output
// This is done using a behavioral 4-to-1 MUX model.

/*
 Inputs:
 rd_addr_data_X: output signals from STP, EVP, or EVB
 rd_addr_data_cur: current read address in FSM2
 command: The parse 2 bit command string from get_command
 rst: testbench reset
 
 Outputs:
 rd_addr_data_updated: The new read address in FSM2
 */


`timescale 1ns/1ps

module rd_addr_data_MUX #(parameter buffer_size = 1024)(
 input [log2(buffer_size) - 1 : 0]  rd_addr_data_STP,
 input [log2(buffer_size) - 1 : 0]  rd_addr_data_EVP,
 input [log2(buffer_size) - 1 : 0]  rd_addr_data_EVB,
// input [log2(buffer_size) - 1 : 0]  rd_addr_data_cur,
 input [1 : 0] 		    instr,
 input 			    rst,
						      
 output reg [log2(buffer_size) - 1 : 0] rd_addr_data_updated



			     );
   localparam STP = 2'b00, EVP = 2'b01, EVB = 2'b10, RST = 2'b11;
   //assign rd_addr_data_updated = rst ? (instr[1] ? (instr[0] ? rd_addr_data_cur : rd_addr_data_EVB) : (instr[0] ? rd_addr_data_EVP : rd_addr_data_STP)) : 0;
   // If active reset, set rd addr data to 0, otherwise, set it to the active
   // FSM 3 module's output
   always@(rd_addr_data_STP, rd_addr_data_EVP, rd_addr_data_EVB, /*rd_addr_data_cur,*/ instr, rst)
     begin
	if(!rst)
	  rd_addr_data_updated <= 0;
	else begin
	case(instr)
	  STP:
	    begin
	       rd_addr_data_updated <= rd_addr_data_STP;
	    end
	  EVP:
            begin
               rd_addr_data_updated <= rd_addr_data_EVP;
            end
	  EVB:
            begin
               rd_addr_data_updated <= rd_addr_data_EVB;
            end
	 /* RST:
            begin
               rd_addr_data_updated = rd_addr_data_cur;
            end
	  default:
	    rd_addr_data_updated = rd_addr_data_cur;*/
	endcase // case (command)
	end // else: !if(!rst)
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

endmodule // rd_addr_data_MUX
