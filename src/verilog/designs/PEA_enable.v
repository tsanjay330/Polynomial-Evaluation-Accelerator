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
    	input [log2(buffer_size) - 1 : 0] command_pop,
    	input [log2(buffer_size) - 1 : 0] data_pop,
    	input [log2(buffer_size) - 1 : 0] result_free_space,
    	input [log2(buffer_size) - 1 : 0] status_free_space,
	input [2 : 0] 			  mode, // How many bits/modes for the FSM operation - 3 bits for 6 operations.
    	input [4 : 0] 			  b, // Second argument of the command token
    	input [3 : 0] 			  N, // The degree of a current/specified coefficient vector
	output reg 			  enable

    );
   
    // Temporary names for State Transition Diagram/Table
    localparam STATE_GET_COMMAND = 3'b000, STATE_STP = 3'b001, STATE_EVP = 3'b010, STATE_EVB = 3'b011, STATE_RST = 3'b100, STATE_OUTPUT = 3'b101;
   
    always @(mode, command_pop, data_pop, result_free_space, status_free_space, b)
    begin
        case (mode)
        STATE_GET_COMMAND:
        begin
            if (command_pop >= 1)
                enable <= 1;
            else
                enable <= 0;
        end
        STATE_STP: begin
	    	if(data_pop >= N + 1)
	    		enable <= 1;
	    	else
	    		enable <= 0;
	end
	STATE_EVP: begin
	    	if(data_pop >= 1)
	    		enable <= 1;
	    	else
	    		enable <= 0;
	end
	STATE_EVB: begin
	    	if(data_pop >= b)
	    		enable <= 1;
	    	else
	    		enable <= 0;
	end
	// Not too sure how enable for RST should be implemented - must interrupt any-inprogress computation
	STATE_RST: enable <= 1; 
        STATE_OUTPUT:
        begin
           if (result_free_space >= b && status_free_space >= b)
           	enable <= 1;
	   else
	   	enable <= 0;
	   
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
