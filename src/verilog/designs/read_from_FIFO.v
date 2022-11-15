`timescale 1ns / 1ns
module read_from_FIFO.v #(parameter word_size = 16, buffer_size = 1024)
(
 input clk,
 input enable,
 input [1:0] cur_state, // This represents the current FSM1 state

 output command_rd_en,
 output data_rd_en,
 
);

   // These State names may change - but they are definitely not the Actor Modes/States
localparam STATE_IDLE = 2'b00, STATE_FIRING_START = 2'b01, STATE_FIRING_WAIT = 2'b10;
   reg 	c_rd_en = 1'b0;
   reg d_rd_en = 1'b0;
   
   
   always@(posedge clock) begin
	if(enable) begin
	  case(cur_state)
            STATE_IDLE: begin // If enabled in idle, need to read from FIFOs
	       c_rd_en <= 1'b1;
	       d_rd_en <= 1'b1;
            end
	    default: begin
	       c_rd_en <= 1'b0; // Otherwise - don't read token from FIFOs
	       d_rd_en <= 1'b0;
	    end
   	  endcase // case (cur_state)  
   	end
   end

endmodule; // read_from_FIFO
