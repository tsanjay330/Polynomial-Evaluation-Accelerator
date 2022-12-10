/*******************************************************************
Name: PEA top level module
Description: The top level module for the PEA
Sub modules: firing_state_FSM2.v
Input Ports:
        clk: clock
        rst: reset    
        control_in: control token from the Command Input FIFO
        data_in: control token from the Data Input FIFO
        invoke: wire to activate invoke signal
        next_instr: next instruction mode 
        control_pop: population of the Control Input FIFO
        data_in: data token from the Data Input FIFO
        data_pop: population of the Data Input FIFO
Output Ports:   rd_in_command: read signal to read a command token
        rd_in_data: read signal to read a data token
        FC: output wire leading to testbench
        wr_out: write output signal
        data_out_result: result token to be written to Result Output FIFO
        data_out_status: status token to be written to Status Output FIFO
        instr: mode to store instruction
        arg2: value of b
        wr_addr_command: write address of input command
        rd_addr_command: read address of input command
        wr_addr_data: write address of input data
        rd_addr_data: read address of input data
Parameters: control_size: bit width of each control token (16 bits, 8 for command, 3 for arg1, 5 for arg2)
States: 
IDLE: mode to start the FSM with the idle state
STATE_FIRING_START: mode where the token is starting firing
STATE_FIRING_WAIT: mode where the token waits after firing        
 ********************************************************************/
`timescale 1ns / 1ns
module PEA_top_module_1 #(parameter word_size = 16, buffer_size = 1024)(
    input clk, rst, // This rst is NOT the rst command.
    input [word_size - 1 : 0] command_in,
    input [word_size - 1 : 0] data_in,
    input invoke,
    input [1 : 0] next_instr,
    input [log2(buffer_size) - 1 : 0] data_pop,
    input [log2(buffer_size) - 1 : 0] command_pop,
    output rd_in_command,
    output rd_in_data,
    output FC,
    output wr_out,
    output [2*word_size - 1 : 0] data_out_result,
    output [2*word_size - 1 : 0] data_out_status,
    output [7:0] instr,
    output [4 : 0] arg2,
	output [log2(buffer_size) - 1 : 0] wr_addr_command,
    output [log2(buffer_size) - 1 : 0] rd_addr_command,
    output [log2(buffer_size) - 1 : 0] wr_addr_data,
    output [log2(buffer_size) - 1 : 0] rd_addr_data); 
            

    localparam STATE_IDLE = 2'b00, STATE_FIRING_START = 2'b01, STATE_FIRING_WAIT = 2'b10;


    reg [2:0] state_module;
    reg [2:0] next_state_module;
    reg start_in;
    wire done_out;
    wire rst_instr;
    assign FC = done_out;

firing_state_FSM2 #(.word_size(word_size))
           FSM2(.clk(clk), .rst(rst), .data_in(data_in),
                .command_in(command_in), .start_fsm2(start_in),
                .next_instr(next_instr),
                .pop_in_fifo_data(data_pop), .pop_in_fifo_command(command_pop),
                .rst_instr(rst_instr), .instr(instr),
                .en_rd_fifo_data(rd_in_data),.en_rd_fifo_command(rd_in_command),
                .done_fsm2(done_out), .en_wr_output_fifo(wr_out),
                .result(data_out_result),.status(data_out_status), .arg2(arg2), 
				.wr_addr_command(wr_addr_command),
				.rd_addr_command(rd_addr_command),
				.wr_addr_data(wr_addr_data),
                .rd_addr_data(rd_addr_data));

   /* Update current state */
    always@(posedge clk, rst_instr)
        begin
            if(!rst || !rst_instr)
                state_module <= STATE_IDLE;
            else
                state_module <= next_state_module;
        end

   /* State evolution of the top-level FSM for this module */
    always@(state_module, invoke, done_out)
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
                    if(done_out)
                        next_state_module <= STATE_IDLE;
                    else
                        next_state_module <= STATE_FIRING_WAIT;
                end

                default: next_state_module <= STATE_IDLE;
            endcase
    end

   /* start_in signal assignment */
    always @(state_module)
    begin
        case (state_module)
            STATE_IDLE:  start_in <= 0;

            STATE_FIRING_START: start_in <= 1;

            STATE_FIRING_WAIT: start_in<= 0;

            default: start_in <= 0;
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


