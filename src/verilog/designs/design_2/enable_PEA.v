/**************************************************
Port List Description

Input Ports
result_free_space - free space output FIFO(result)
status_free_space - free space output FIFO(status)
next_mode_in - next state mode for instruction
mode - next state mode for all instructions
arg2 - value of b

Output Ports 
wr_addr_command - write address signal for input command
rd_addr_command - read address signal for input command
wr_addr_data - write address signal for input data
rd_addr_data - read address signal for input data
enable - output wire set to stream tokens

STATES
SETUP_INSTR - mode activated which includes difference of wr_addr - rd_addr
INSTR - mode which activates when independent instructions are enabled

STP - mode enabled to store command
EVP - mode enabled to evaluate monomial
EVB - mode enabled to evaluate polynomial
RST - mode enabled to reset the FIFO
**************************************************/
`timescale 1ns / 1ps
module PEA_enable #(parameter word_size = 16,  buffer_size = 1024, 
					buffer_size_out = 32)
   (
        input [log2(buffer_size_out) - 1 : 0] result_free_space,
        input [log2(buffer_size_out) - 1 : 0] status_free_space,
        input [1 : 0] next_mode_in,
        input [7 : 0] mode,
        input [4 : 0] arg2,
		input [log2(buffer_size) - 1 : 0] wr_addr_command,
        input [log2(buffer_size) - 1 : 0] rd_addr_command,
		input [log2(buffer_size) - 1 : 0] wr_addr_data,
        input [log2(buffer_size) - 1 : 0] rd_addr_data,
        output reg enable
		);

    localparam SETUP_INSTR = 2'b00, INSTR = 2'b01;

    // Temporary names for State Transition Diagram/Table
    localparam STP = 8'd0, EVP = 8'd1,
               EVB = 8'd2, RST = 8'd3;


    always @(*)
//wr_addr_command,rd_addr_command,wr_addr_data,rd_addr_data, next_mode_in,mode, result_free_space, status_free_space, arg2)
    begin
    case(next_mode_in)
        SETUP_INSTR: begin
                if ((wr_addr_command - rd_addr_command) >= 1)
                    enable <= 1;
                else
                    enable <= 0;
        end

        INSTR: begin
            case(mode)
                STP: begin
                    if((wr_addr_data - rd_addr_data) >= arg2)  //&& result_free_space >= 1
                        enable <= 1;
                    else
                        enable <= 0;
                end

                EVP: begin
                    if((wr_addr_data - rd_addr_data) >= 1) //&& result_free_space >= 1
                        //&& status_free_space >= 1)
                        enable <= 1;
                    else
                        enable <= 0;
                end

                EVB: begin
                    if( (wr_addr_data - rd_addr_data) >= arg2) //&& result_free_space >= arg2
                       //&& status_free_space >= arg2)
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

        default: begin
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
