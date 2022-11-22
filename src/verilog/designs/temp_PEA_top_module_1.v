/*********************************************************************
Name: PEA top level module
Description: The top level module for the PEA
Sub modules: PEA_enable,
Input Ports:    control_in: control token from the Control Input FIFO
        control_pop: population of the Control Input FIFO
        data_in: data token from the Data Input FIFO
        data_pop: population of the Data Input FIFO
        result_free_space: remaining empty words of the Result Output FIFO
        status_free_space: remaining empty words of the Status Output FIFO
Output Ports:   control_rd_en: read enable signal to read a control token
        data_rd_en: read enable signal to read a data token
        result_wr_en: write enable signal for Result Output FIFO
        status_wr_en: write enable signal for Status Output FIFO
        result_out: result token to be written to Result Output FIFO
        status_out: status token to be written to Status Output FIFO
Parameters: control_size: bit width of each control token (16 bits, 8 for command, 3 for arg1, 5 for arg2)
        data_size: bit width of each data token (16 bits signed 2's comp)
        result_size: bit width of each result token (32 bits signed 2's comp)
        status_size: bit width of each status token (??? - need to determine how many status cases/errors there should be)
        buffer_size: Total # of words in each FIFO (assumed to be equal for all FIFOs)
        
 ********************************************************************/
`timescale 1ns / 1ns
module PEA_top_module_1 #(parameter word_size = 16, buffer_size = 1024)(
    input clk, rst, // This rst is NOT the rst command.
    input [word_size - 1 : 0] command_in,
    input [word_size - 1 : 0] data_in,
    input invoke,
    input [1 : 0] next_mode_in,
    //output [1 : 0] next_mode_out,
    output rd_in_command,
    output rd_in_data,
    output FC;
    /*output wr_out_result;
	output wr_out_status;*/
	output [2*word_size - 1 : 0] data_out_result,
    output [2*word_size - 1 : 0] data_out_status,
	output [2:0] mode,//current mode needed for enable
    output [4 : 0] b, // Argument 2 of current command input token, gets used
    output [3 : 0] N
    // Degree/Length of a current/specified coefficient vector(can be a total of value=8)
            );

    localparam STATE_IDLE = 2'b00, STATE_FIRING_START = 2'b01, STATE_FIRING_WAIT = 2'b10;

   /*   Persistent local variables (from lide_c_pea_context_struct) */
    // S, the coffeicient vectors reg, is 3D with 8 rows & 11 columns (10 degrees, including degree 0) of 16-bit signed two's complement values
    reg [word_size - 1 : 0] S[7 : 0][10:0];

    // N, the array of polynomial degrees, is 2D with 8 rows of 4-bit unsigned values. 4'b1111 will represent the initial state before STP.
    reg [3 : 0]  N[7:0];

    // A, the current/specified index/address for the S array (3 bits b/c log2(length(S) = 3)
    reg [2 : 0]  A;

    // B, the number of operations to perform
    reg [4 : 0]  B;

    // Temporary result token - might not be needed
    reg [2*word_size - 1 : 0] result_temp;

    // Temporary error token - might not be needed
    reg [2*word_size - 1 : 0] status_temp;

    reg [2:0] state_module;
    reg [2:0] next_state_module;
    reg start_in_child;//Taken from lab07(might not use the same)
    wire done_out_child;//Taken from lab07(might not use the same)


   reg [1 : 0]               mode;
   assign FC = done_out_child;
  // wire                enable;
    /*firing state FSM2-module instantiation block*/
   firing_state_FSM2 #(.word_size(word_size))
           FSM2(clk, rst, data_in, command_in, start_in, next_mode_in, data_rd_en, command_rd_en, next_mode_out, done_out_child, result_wr_en, status_wr_en, result_out, status_out);
   /* Update current state */
   always@(posedge clk)
     begin
    if(!rst)
      state_module <= STATE_IDLE;
    else
      state_module <= next_state_module;
     end

   /* State evolution of the top-level FSM for this module */
always@(state_module, invoke, done_out_child)
  begin
    case(state_module)
    STATE_IDLE: begin
    if(invoke)
      next_state_module <= STATE_FIRING_START;
    else
      next_state_module <= STATE_IDLE;
    end
    STATE_FIRING_START: begin
           next_state_module <= STATE_FIRING_WAIT;
        end
    STATE_FIRING_WAIT: begin
        if(done_out_child)
          next_state_module <= STATE_IDLE;
        else
          next_state_module <= STATE_FIRING_WAIT;
        end
    default:
      next_state_module <= STATE_IDLE;
    endcase

  end // always@ (state_module, invoke, done_out_child)

   /* start_in_child signal assignment */
   always @(state_module) begin
    case (state_module)
      STATE_IDLE:  start_in_child <= 0;
          STATE_FIRING_START: start_in_child <= 1;
          STATE_FIRING_WAIT: start_in_child <= 0;
          default: start_in_child <= 0;
        endcase
   end // always @ (state_module)

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

~                             
