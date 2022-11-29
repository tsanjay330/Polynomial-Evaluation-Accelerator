`timescale 1ns / 1ps
module PEA_enable #(parameter word_size = 16,  buffer_size = 1024)
   (
        input [log2(buffer_size) - 1 : 0] command_pop,
        input [log2(buffer_size) - 1 : 0] data_pop,
        input [log2(buffer_size) - 1 : 0] result_free_space,
        input [log2(buffer_size) - 1 : 0] status_free_space,
        input [1 : 0] next_mode_in,
        input [7 : 0] mode,
        input [4 : 0] b, // Second argument of the command token
        input [3 : 0] N, // The degree of a current/specified coefficient vector
        output reg enable

    );

    localparam SETUP_INSTR = 2'b00, INSTR = 2'b01;

    // Temporary names for State Transition Diagram/Table
    localparam STP = 8'd0, EVP = 8'd1, 
			   EVB = 8'd2, RST = 8'd3;

    always @(next_mode_in,mode, command_pop, data_pop, result_free_space,
            status_free_space, b)
    begin
    case(next_mode_in)
        SETUP_INSTR: begin
            	if (command_pop >= 1)
                    enable <= 1;
                else
                    enable <= 0;
        end

		INSTR: begin
			case(mode)
            	STP: begin
                	if(data_pop >= N + 1 && result_free_space >= 1 
						&& status_free_space >= 1)
                    	enable <= 1;
                	else
                    	enable <= 0;
            	end

            	EVP: begin
                	if(data_pop >= 1 && result_free_space >= b 
					    && status_free_space >= b)
                    	enable <= 1;
                	else
                    	enable <= 0;
            	end

            	EVB: begin
                	if(data_pop >= b && result_free_space >= b 
						&& status_free_space >= b)
                    	enable <= 1;
                	else
                    	enable <= 0;
            	end

            	RST: enable <= 1;

            	default: begin
                	 enable <= 0;
            	end

            endcase
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
    endfunction // log2
endmodule

