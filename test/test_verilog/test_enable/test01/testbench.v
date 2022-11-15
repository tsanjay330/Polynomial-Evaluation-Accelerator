`timescale 1ns / 1ps
module testbench();
    parameter word_size = 16, buffer_size = 1024;
    parameter GET_COMMAND = 3'b000, STP = 3'b001, EVP = 3'b010, EVB = 3'b011, OUTPUT = 3'b100, RST = 3'b101;

// Inputs to enable module (all wires here are outputs from FIFO modules)
   reg clk;
   reg rst;
   reg [log2(buffer_size) - 1 : 0] command_pop;
   reg [log2(buffer_size) - 1 : 0] data_pop;
   wire [log2(buffer_size) - 1 : 0] result_free_space;
   wire [log2(buffer_size) - 1 : 0] status_free_space;
   reg rd_en_fifo_data;
   reg [2 : 0] mode;//3-bit operations mode
   reg [2 : 0] next_mode;
   reg [4 : 0] b;
   reg [3 : 0] N;
/*   reg [word_size - 1 : 0] command_in;
   reg [word_size - 1 : 0] data_in;*/
// Outputs to enable module
   wire enable;
// Inputs to FIFO modules
   reg wr_en_input;
   reg result_wr_en, result_rd_en;
   reg [word_size - 1 : 0] result_data_in;
   wire [log2(buffer_size - 1 : 0] free_space_fifo_data, free_space_fifo_command;
   wire [log2(buffer_size - 1 : 0] free_space_out_fifo_result, free_space_out_fifo_status;
   wire [word_size - 1 : 0] data_in_fifo_data, data_in_fifo_command;
   wire [2*word_size - 1 : 0] data_out_fifo_result, data_out_fifo_status;
   
   integer i;
   
/******************************************************
 Instantiate 4 FIFOs and the enable module for the PEA
 *******************************************************/
/*Haven't included rd_en_input signals yet in the port list*/
  fifo #(word_size, buffer_size) fifo_in_data(clk, rst, wr_en_input, data_pop, data_in, free_space_fifo_data, data_in_fifo_data);

  fifo #(word_size, buffer_size) fifo_in_command(clk, rst, wr_en_input, command_pop, command_in, free_space_fifo_command, data_in_fifo_command);
 
  fifo #(2*word_size, buffer_size) fifo_out_result(clk, rst, rd_en_fifo_data, free_space_out_fifo_result, data_out_fifo_result);
 
  fifo #(2*word_size, buffer_size) fifo_out_status(clk, rst, rd_en_fifo_data, free_space_out_fifo_status, data_out_fifo_status);
  
/*****************************************************
Instantiation of enable module
*****************************************************/
   PEA_enable #(word_size, buffer_size) enable(rst, control_pop, data_pop, result_free_space, status_free_space, mode, control_in, enable);

  integer descr; 
   
   
initial begin
   // Clock alternates with a period of 2 time units
   for(i = 0; i < 200; i = i + 1) begin
      #1 clk <= 1;
      #1 clk <= 0;
   end

end

initial 
begin
    #1;
    rst <= 0;
    wr_en_input <= 0;
    data_pop <= 0;
    command_pop <= 0;
    next_mode <= GET_COMMAND;
    rd_en_fifo_data <= 0;
    #2 rst <= 1;
    #2;

    $fdisplay(descr, "Setting input fifos");
   
   



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
    endfunction

endmodule
