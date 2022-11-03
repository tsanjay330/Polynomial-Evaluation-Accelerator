/****************************************************************
Parameters:     control_size: bit width of each control token (16 bits, 8 for comm and, 3 for arg1, 5 for arg2)
                data_size: bit width of each data token (16 bits signed 2's comp)
                result_size: bit width of each result token (32 bits signed 2's comp)
                status_size: bit width of each status token (??? - need to determine how many status cases/errors there should be)
                buffer_size: Total # of words in each FIFO (assumed to be equal for all FIFOs)
 
 
 
*****************************************************************/
`timescale 1ns / 1ps
module PEA_enable #(parameter word_size = 16,  buffer_size = 1024)
   (
	input 				  rst,
    	input [log2(buffer_size) - 1 : 0] control_pop,
    	input [log2(buffer_size) - 1 : 0] data_pop,
    	input [log2(buffer_size) - 1 : 0] result_free_space,
    	input [log2(buffer_size) - 1 : 0] status_free_space,
	input [1 : 0] 			  mode, // How many bits/modes for the FSM operation?
	input [word_size - 1 : 0] control_in, // the current command read in from input FIFO
	output reg 			  enable


    );
   
    // Temporary names for State Transition Diagram/Table
    localparam GET_COMMAND = 2'b00, COMP = 2'b01, OUTPUT = 2'b10;
   localparam STP = 2'b00, EVP = 2'b01, EVB = 2'b10, RST = 2'b11;

   reg [7:0] 				  instr;
   reg [2:0] 				  arg1;
   reg [4:0] 				  arg2;
   {instr, arg1, arg2} = control_in;
   
    always @(mode, control_pop, data_pop, result_free_space, status_free_space, control_in)
    begin
        case (mode)
        GET_COMMAND:
        begin
            if (control_pop >= 1)
                enable <= 1;
            else
                enable <= 0;
        end
        COMP:
        begin
	   // Before this is implemented, we need to know exactly which 8-bit values correspond to which mode
	   // Flow:
	    case({instr[1], instr[0]})
	   	STP: begin
	    		if(data_pop >= arg2 + 1)
	    			enable <= 1;
	    		else
	    			enable <= 0;
	    	end
	    	EVP: begin
	    		if(data_pop >= 1)
	    			enable <= 1;
	    		else
	    			enable <= 0;
	    	end
	    	EVB: begin
	    		if(data_pop >= arg2)
	    			enable <= 1;
	    		else
	    			enable <= 0;
	    	end
	    	RST: // Not too sure how enable for RST should be implemented - must interrupt any-inprogress computation
	    		enable <= 1;
	    	default:
	    		enable <= 0;
	    endcase 
        end
        OUTPUT:
        begin
           if (result_free_space >= 1 && status_free_space >= 1)
                enable <= 1;
        end
        default:
        begin
            enable <= 0;
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
    endfunction // log2
endmodule
