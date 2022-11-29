/******************************************************************************
INPUT PORTS

data_in - data from input fifo data
command_in - data from input fifo command
start_fsm2 - nested FSM start signal form parent FSM
next_mode_in - mode to determine the next FSM state

OUTPUT PORTS

instr - The instruction reg within every FSM state
rd_in_data - read enable signal for input fifo data
rd_in_command - read enable signal for input fifo command
done_fsm2 - nested FSM end signal to aprent FSM
wr_out_result - output result fifo write enable signal
wr_out_status - output status fifo write enable signal
data_out_result - output data for writing into output result fifo
data_out_status - output data for writing into output status fifo
*******************************************************************************/
`timescale 1ns/1ps
module firing_state_FSM2
        #(parameter word_size = 16, buffer_size = 1024)
        (input clk, rst,
        input [word_size - 1 : 0] data_in,
        input [word_size - 1 : 0] command_in,
        input start_fsm2,
        input [1 : 0] next_mode_in,
		output reg [7:0] instr,
        output en_rd_fifo_data,
        output en_rd_fifo_command,
        output reg done_fsm2,
        output reg en_wr_output_fifo,
        output reg [word_size - 1 : 0] result,
        output reg [word_size - 1 : 0] status);
   
	localparam SETUP_INSTR = 2'b00, INSTR = 2'b01, OUTPUT = 2'b10;

	localparam GET_COMMAND=3'b000, STP=3'b001, EVP=3'b010, 
		EVB=3'b011, OUTPUT=3'b100, RST=3'b101;

	localparam STATE_START=4'b0000, STATE_GET_COMMAND_START=4'b0001, 
		STATE_GET_COMMAND_WAIT=4'b0010, STATE_STP_START=4'b0011, 
		STATE_STP_WAIT=4'b0100, STATE_EVP_START=4'b0101, 
		STATE_EVP_WAIT=4'b0110, STATE_EVB_START=4'b0111, 
		STATE_EVB_WAIT=4'b1000, STATE_RST=4'b1001, STATE_OUTPUT=4'b1010;

   reg [3 : 0] state_module, next_state_module;
   reg en_get_command;
   reg en_stp;
   reg en_evp;
   reg en_evb;
   reg en_rst;
   reg done_out_get_command;
   reg done_out_stp;
   reg done_out_evp;
   reg done_out_evb;
   reg done_out_rst;
   wire [2:0] arg1;
   wire [4:0] arg2; 
   wire [1:0] err_out;
   wire [log2(buffer_size) - 1 : 0] rd_addr_data, rd_addr_command;
   wire [7:0] rd_addr_S;
   wire [2:0] rd_addr_N;
   wire [log2(buffer_size) - 1 : 0] wr_addr_data, wr_addr_command, wr_addr_S, wr_addr_N;
   wire ram_in_S, ram_in_N, ram_in_data, ram_in_command;
//   wire [2*word_size - 1 : 0] result, status;
   wire reset_out;
   wire wr_en_ram_command, wr_en_ram_data, wr_en_ram_S, wr_en_ram_N;
   wire rd_en_ram_command, rd_en_ram_data, rd_en_ram_S, rd_en_ram_NN;
   wire [word_size - 1 : 0] ram_out_command, ram_out_data, ram_out_S, ram_out_N;
//   wire rd_out_N, en_ram_out_N;(maybe rename to wr_data_suc and rd_data_suc)
/****************************************************************
Instantiation of RAM modules
****************************************************************/
single_port_ram #(.word_size(word_size), .buffer_size(buffer_size))
	RAM_COMMAND(.command(ram_in_command), .addr(wr_addr_command), .rd_addr(rd_addr_command), .wr_en(wr_en_ram_command), 
	.rd_en(rd_en_ram_command), .clk(clk), .q(ram_out_command));

single_port_ram #(.word_size(word_size), .buffer_size(buffer_size))
	RAM_DATA(.data(ram_in_data), .addr(wr_addr_data), .rd_addr(rd_addr_data), .wr_en(wr_en_ram_data), .rd_en(rd_en_ram_data),
	.clk(clk), .q(ram_out_data));

single_port_ram #(.word_size(word_size), .buffer_size(buffer_size))
	RAM_S(.data(ram_in_S), .addr(wr_addr_S), .rd_addr(rd_addr_S), .wr_en(wr_en_ram_S), .rd_en(rd_en_ram_S), .clk(clk), .q(ram_out_S));

/*	RAM_N((what should this be? arg2, which is sometimes N?), wr_addr, rd_addr,
			wr_en_ram_N, rd_en_ram_NN, clk, ram_out_N);*/
N_ram #(.word_size(word_size), .buffer_size(buffer_size))
    RAM_N(.data(ram_in_N), .rst(rst), .wr_addr(wr_addr_N), .rd_addr(rd_addr_N), .wr_en(wr_en_ram_N), .re_en(rd_en_ram_N), .clk(clk), .q(ram_out_N), .wr_suc(wr_suc_data), .q_en(rd_data_suc));

mem_controller #(.word_size(word_size), .buffer_size(buffer_size))
    mem_load_controller(.clk(clk), .rst(rst), 
//Would need help to figure this module out during class

/***********************************************************************
Instantiation of the nested FSM for get_command_FSM3, STP, EVP, EVB, RST
***********************************************************************/
/*Might need to add functionality to get_command if error is non-zero*/
get_command_FSM_3 #()
          get_command(.clk(clk), .rst(rst), .start_get_cmd(en_get_cmd), .command_in(ram_out_command), .en_rd_cmd(rd_en_ram_command), .done_get_cmd(done_out_get_command), .instr(instr), .arg1(arg1), .arg2(arg2), .error(error_out));

STP_FSM_3 #(.buffer_size(buffer_size))
      stp_command(.clk(clk), .rst(rst), .start_stp(en_stp), .rd_addr_data(rd_addr_data), .A(arg1), .N(arg2), .next_c(ram_out_S), .done_stp(done_out_stp), .en_rd_data(rd_en_ram_data), .en_wr_S(wr_en_ram_S), .rd_addr_data_updated(rd_addr_data), .wr_addr_S(wr_addr_S), .c(ram_in_S), .result(result), .status(status)  

EVP_FSM_3 #(.buffer_size(buffer_size))
       evp_command(.clk(clk), .rst(rst), .start_evp(en_evp), .A(arg1), .x(ram_out_data), .c_i(ram_out_S), .rd_addr_data(rd_addr_data), .en_rd_data(rd_en_ram_data), .en_rd_S(rd_en_ram_S), .en_rd_N(rd_en_ram_N), .rd_addr_data_updated(rd_addr_data), .rd_addr_S(rd_addr_S), .done_evp(done_out_evp), .result(result), .status(status); 
EVB_FSM_3 #(.buffer_size(buffer_size))
       evb_command(.clk(clk), .rst(rst), .start_evb(en_evb), .A(arg1), .b(arg2), .x_b(ram_out_data), .c_i(ram_out_S), .N(ram_out_N), .rd_addr_data(rd_addr_data), .done_evb(done_out_evb), .en_rd_data(rd_en_ram_data), .en_rd_S(rd_en_ram_S), .en_rd_N(ed_en_ram_N), .rd_addr_data_updated(rd_addr_data), .rd_addr_S(rd_addr_S), .result(result), .status(status);  
RST_FSM_3 #(.buffer_size(buffer_size))
       rst_command(.clk(clk), .start_rst(en_rst), .rst(reset_out), .done_rst(done_out_rst));


always @(posedge clk or negedge rst)
begin
    if(!rst)
    begin
        state_module <= STATE_START;
        end
        else
        begin
            state_module <= next_state_module;
        end
end

always @(state_module, start_fsm2, done_out_get_command, done_out_stp, done_out_evp, done_out_evb, done_out_rst, next_mode_in)
begin
case(state_module)
    STATE_START:
    begin
        case(next_mode_in)
            SETUP_INSTR: begin
                next_state_module <= STATE_GET_COMMAND_START;
            end

            INSTR: begin
                case(instr)//same mode signal that is passed to enable 
                    STP: begin
                        next_state_module <= STATE_STP_START;
                    end

                    EVP: begin
                        next_state_module <= STATE_EVP_START;
                    end

                    EVB: begin
                        next_state_module <= STATE_EVB_START;
                    end

                    //RST??
                endcase
            end

            OUTPUT: begin
            next_state_module <= STATE_OUTPUT;
            end

            default:
            begin
                next_state_module <= STATE_START;
            end
        endcase
    end	
/***************************************
CFDF: firing mode_GET_COMMAND
***************************************/
    STATE_GET_COMMAND_START:
    begin
        next_state_module <= STATE_GET_COMMAND_WAIT;
    end
    
    STATE_GET_COMMAND_WAIT:
    begin
        if(done_out_get_command)
        begin
            next_state_module <= STATE_START;
        end
        else
        begin
            next_state_module <= STATE_GET_COMMAND_WAIT;
        end
    end

/*********************************************
CFDF: firing mode STP
*********************************************/
    STATE_STP_START:
    begin
        next_state_module <= STATE_STP_WAIT;
    end

    STATE_STP_WAIT:
    begin
        if(done_out_stp)
        begin
            next_state_module <= STATE_START;
        end
        else
        begin
            next_state_module <= STATE_STP_WAIT;
        end
    end

/***********************************************
CFDF: firing mode EVP
***********************************************/
    STATE_EVP_START:
    begin
        next_state_module <= STATE_EVP_WAIT;
    end

    STATE_EVP_WAIT:
    begin
        if(done_out_evp)
        begin
            next_state_module <= STATE_START;
        end
        else
        begin
            next_state_module <= STATE_EVP_WAIT;
        end
    end

/************************************************
CFDF: firing mode EVB
************************************************/
    STATE_EVB_START:
    begin
        next_state_module <= STATE_EVB_WAIT;
    end

    STATE_EVB_WAIT:
    begin
        if(done_out_evb)
        begin
            next_state_module <= STATE_START;
        end
        else
        begin
            next_state_module <= STATE_EVB_WAIT;
        end
    end
/********************************************
CFDF: mode RST
********************************************/
    STATE_RST:
    begin
        next_state_module <= STATE_START;
    end
/***********************************************
CFDF: firing mode OUTPUT
***********************************************/
    STATE_OUTPUT:
    begin
        next_state_module <= STATE_END;
    end

    STATE_END:
    begin
        next_state_module <= STATE_START;
    end

  //Still unsure if RST would work like this or not?? 
    default:
    begin
        next_state_module <= STATE_START;
    end
    endcase
end

/**************************************
OUTPUT SIGNALS
**************************************/
always @(state_module)
begin
    case(state_module)
    STATE_START:
    begin
        en_wr_output_fifo <= 0;
        en_get_command <= 0;
        en_stp <= 0;
        en_evp <= 0;
        en_evb <= 0;
        en_rst <= 0;
        done_fsm2 <= 0;
    end

    STATE_GET_COMMAND_START:
    begin
        en_wr_output_fifo <= 0;
        en_get_command <= 1;
        en_stp <= 0;
        en_evp <= 0;
        en_evb <= 0;
        en_rst <= 0;
        done_fsm2 <= 0;
    end

    STATE_GET_COMMAND_WAIT:
    begin
        en_wr_output_fifo <= 0;
        en_get_command <= 0;
        en_stp <= 0;
        en_evp <= 0;
        en_evb <= 0;
        en_rst <= 0;
        done_fsm2 <= 0;
    end
    STATE_STP_START:
    begin
        en_wr_output_fifo <= 0;
        en_get_command <= 0;
        en_stp <= 1;
        en_evp <= 0;
        en_evb <= 0;
        en_rst <= 0;
        done_fsm2 <= 0;
    end
    STATE_STP_WAIT:
    begin
        en_wr_output_fifo <= 0;
        en_get_command <= 0;
        en_stp <= 0;
        en_evp <= 0;
        en_evb <= 0;
        en_rst <= 0;
        done_fsm2 <= 0;
    end
    STATE_EVP_START:
    begin
        en_wr_output_fifo <= 0;
        en_get_command <= 0;
        en_stp <= 0;
        en_evp <= 1;
        en_evb <= 0;
        en_rst <= 0;
        done_fsm2 <= 0;
    end
    STATE_EVP_WAIT:
    begin
        en_wr_output_fifo <= 0;
        en_get_command <= 0;
        en_stp <= 0;
        en_evp <= 0;
        en_evb <= 0;
        en_rst <= 0;
        done_fsm2 <= 0;
    end
    STATE_EVB_START:
    begin
        en_wr_output_fifo <= 0;
        en_get_command <= 0;
        en_stp <= 0;
        en_evp <= 0;
        en_evb <= 1;
        en_rst <= 0;
        done_fsm2 <= 0;
    end
    STATE_EVB_WAIT:
    begin
        en_wr_output_fifo <= 0;
        en_get_command <= 0;
        en_stp <= 0;
        en_evp <= 0;
        en_evb <= 0;
        en_rst <= 0;
        done_fsm2 <= 0;
    end
    STATE_RST:
    begin
        en_wr_output_fifo <= 0;
        en_get_command <= 0;
        en_stp <= 0;
        en_evp <= 0;
        en_evb <= 0;
        en_rst <= 1;
        done_fsm2 <= 0;
    end

    STATE_OUTPUT:
    begin
        en_wr_output_fifo <= 1;
        en_get_command <= 0;
        en_stp <= 0;
        en_evp <= 0;
        en_evb <= 0;
        en_rst <= 0;
        done_fsm2 <= 0;
    end

//Should done_fsm2 or en_wr_output_fifo be written at all in this block??

    default:
    begin
        en_wr_output_fifo <= 0;
        en_get_command <= 0;
        en_stp <= 0;
        en_evp <= 0;
        en_evb <= 0;
        en_rst <= 0;
        done_fsm2 <= 0;
    end
    endcase
end
function integer log2;
input [31 : 0] value;
integer i;
begin
    if(value == 1)
       i=1;
    else
       begin
       i=value-1;
       for(log2=0;i>0;log2=log2+1) begin
            i=i>>1;
       end
       end
end
endfunction
endmodule       
            
