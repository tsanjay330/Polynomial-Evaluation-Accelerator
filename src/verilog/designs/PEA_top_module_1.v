/*********************************************************************
Name: PEA top level module
Description: The top level module for the PEA
Sub modules: PEA_enable,
Input Ports: 	control_in: control token from the Control Input FIFO
 		control_pop: population of the Control Input FIFO
 		data_in: data token from the Data Input FIFO
 		data_pop: population of the Data Input FIFO
 		result_free_space: remaining empty words of the Result Output FIFO
 		status_free_space: remaining empty words of the Status Output FIFO
Output Ports:	control_rd_en: read enable signal to read a control token
 		data_rd_en: read enable signal to read a data token
 		result_wr_en: write enable signal for Result Output FIFO
 		status_wr_en: write enable signal for Status Output FIFO
 		result_out: result token to be written to Result Output FIFO
 		status_out: status token to be written to Status Output FIFO
Parameters:	control_size: bit width of each control token (16 bits, 8 for command, 3 for arg1, 5 for arg2)
 		data_size: bit width of each data token (16 bits signed 2's comp)
 		result_size: bit width of each result token (32 bits signed 2's comp)
 		status_size: bit width of each status token (??? - need to determine how many status cases/errors there should be)
 		buffer_size: Total # of words in each FIFO (assumed to be equal for all FIFOs)
 		
 ********************************************************************/
`timescale 1ns / 1ns
module PEA_top_module_1 #(parameter word_size = 16, buffer_size = 1024)
	input clk, rst,
	input [word_size - 1 : 0] control_in,
	input [log2(buffer_size) - 1 : 0] command_pop,
  	input [word_size - 1 : 0] data_in,
  	input [log2(buffer_size) - 1 : 0] data_pop,
  	input [log2(buffer_size) - 1 : 0] result_free_space, // should this be log2(buffer_size) bits long?
  	input [log2(buffer_size) - 1 : 0] status_free_space, // same for this
  	output control_rd_en,
	output data_rd_en,
  	output result_wr_en,
  	output status_wr_en,
  	output [2*word_size - 1 : 0] result_out,
  	output [word_size - 1 : 0] status_out,
  	output [4 : 0] b // Argument 2 of current command input token, gets used 
	
			);

   reg [1:0] 			     mode;
   wire 			     enable;


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
   
