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
        #(parameter word_size = 16, buffer_size = 1024, n_size = 8, s_size = 88)
        (input clk, rst,
        input [word_size - 1 : 0] data_in,
        input [word_size - 1 : 0] command_in,
        input start_fsm2,
        input [1 : 0] next_instr,
        input [word_size - 1 : 0] pop_in_fifo_data,
        input [word_size - 1 : 0] pop_in_fifo_command,
		output rst_instr,
		output [7:0] instr,
        output en_rd_fifo_data,
        output en_rd_fifo_command,
        output reg done_fsm2,
        output reg en_wr_output_fifo,
        output [word_size - 1 : 0] result,
        output [word_size - 1 : 0] status,
        output [4 : 0] arg2);
   
	localparam SETUP_INSTR = 2'b00, INSTR = 2'b01, OUTPUT = 2'b10;

	localparam STP=8'd0, EVP=8'd1, EVB=8'd2, RST=8'd3;

	localparam STATE_START=4'b0000, STATE_GET_COMMAND_START=4'b0001, 
		STATE_GET_COMMAND_WAIT=4'b0010, STATE_GET_COMMAND_FINISH=4'b0011,STATE_STP_START=4'b0100, STATE_STP_WAIT=4'b0101, STATE_EVP_START=4'b0110, STATE_EVP_WAIT=4'b0111, STATE_EVB_START=4'b1000, STATE_EVB_WAIT=4'b1001, STATE_EVB_OUTPUT=4'b1010, STATE_RST=4'b1011, STATE_OUTPUT=4'b1100;

   reg [3 : 0] state_module, next_state_module;
   reg en_get_command;
   reg en_stp;
   reg en_evp;
   reg en_evb;
   reg en_rst;
   wire done_out_get_command;
   wire done_out_stp;
   wire done_out_evp;
   wire done_out_evb;
   wire done_out_rst;
   wire [2:0] arg1;
//   wire [4:0] arg2; 
   wire [log2(buffer_size) - 1 : 0] rd_addr_data, rd_addr_command;
   wire [7:0] rd_addr_S;
   wire [2:0] rd_addr_N;
   wire [log2(buffer_size) - 1 : 0] wr_addr_data, wr_addr_command;
   wire [log2(s_size) - 1 : 0] wr_addr_S;
   wire [log2(n_size) - 1 : 0] wr_addr_N;
   wire [word_size - 1: 0] ram_in_S, ram_in_N, ram_in_data, ram_in_command;
   wire wr_en_ram_command, wr_en_ram_data, wr_en_ram_S, wr_en_ram_N;
//   wire wr_en_fifo_result, wr_en_fifo_status;
   wire rd_en_ram_command, rd_en_ram_data, rd_en_ram_S, rd_en_ram_N;
   wire [word_size - 1 : 0] ram_out_command, ram_out_data, ram_out_S, ram_out_N;
/****************************************************************
Instantiation of RAM modules
****************************************************************/
single_port_ram #(.word_size(word_size), .buffer_size(buffer_size))
	RAM_COMMAND(.data(ram_in_command), .addr(wr_addr_command), 
			.rd_addr(rd_addr_command), .wr_en(wr_en_ram_command), 
			.rd_en(rd_en_ram_command), .clk(clk), .q(ram_out_command));

single_port_ram #(.word_size(word_size), .buffer_size(buffer_size))
	RAM_DATA(.data(ram_in_data), .addr(wr_addr_data), .rd_addr(rd_addr_data), 
			.wr_en(wr_en_ram_data), .rd_en(rd_en_ram_data), .clk(clk), 
			.q(ram_out_data));

single_port_ram #(.word_size(word_size), .buffer_size(buffer_size))
	RAM_S(.data(ram_in_S), .addr(wr_addr_S), .rd_addr(rd_addr_S), 
			.wr_en(wr_en_ram_S), .rd_en(rd_en_ram_S), .clk(clk), .q(ram_out_S));

N_ram #(.word_size(word_size), .buffer_size(buffer_size))
    RAM_N(.data(ram_in_N), .rst(rst), .wr_addr(wr_addr_N), .rd_addr(rd_addr_N),
			.wr_en(wr_en_ram_N), .rd_en(rd_en_ram_N), .clk(clk), .q(ram_out_N));

mem_controller #(.word_size(word_size), .buffer_size(buffer_size))
    DATA_MEM_CONTROLLER(.clk(clk), .rst(rst), .rst_instr(rst_instr),
			.FIFO_population(pop_in_fifo_data), .input_token(data_in), .FIFO_rd_en(en_rd_fifo_data), .ram_wr_en(wr_en_ram_data), .ram_wr_addr(wr_addr_data), .output_token(ram_in_data));

mem_controller #(.word_size(word_size), .buffer_size(buffer_size))
    COMMAND_MEM_CONTROLLER(.clk(clk), .rst(rst), .rst_instr(rst_instr), 
			.FIFO_population(pop_in_fifo_command), .input_token(command_in), .FIFO_rd_en(en_rd_fifo_command), .ram_wr_en(wr_en_ram_command), .ram_wr_addr(wr_addr_command), .output_token(ram_in_command));

/***********************************************************************
Instantiation of the nested FSM for get_command_FSM3, STP, EVP, EVB, RST
***********************************************************************/
/*Might need to add functionality to get_command if error is non-zero*/
get_command_FSM_3 #(.buffer_size(buffer_size))
		get_command(.clk(clk), .rst(rst), .rst_instr(rst_instr),
		 .start_get_cmd(en_get_command), 
		.command(ram_out_command), .en_rd_cmd(rd_en_ram_command), 
		.done_get_cmd(done_out_get_command), 
		.rd_addr_command_updated(rd_addr_command), 
		.instr(instr), .arg1(arg1),.arg2(arg2));

STP_FSM_3 #(.word_size(word_size), .buffer_size(buffer_size), .n_size(n_size), .s_size(s_size))
		stp_command(.clk(clk), .rst(rst), .rst_instr(rst_instr), .start_stp(en_stp), .rd_addr_data(rd_addr_data), .A(arg1), .N(arg2), .next_c(ram_out_S), .done_stp(done_out_stp), .en_rd_data(rd_en_ram_data), .en_wr_S(wr_en_ram_S), .en_wr_N(wr_en_ram_N), .rd_addr_data_updated(rd_addr_data), .wr_addr_S(wr_addr_S), .wr_addr_N(wr_addr_N), .c(ram_in_S), .N_out(ram_out_N), .result(result), .status(status), .fifo_wr_en_r(en_wr_output_fifo), .fifo_wr_en_s(en_wr_output_fifo));  
EVP_FSM_3 #(.buffer_size(buffer_size))
		evp_command(.clk(clk), .rst(rst), .rst_instr(rst_instr), .start_evp(en_evp), .A(arg1), .x(ram_out_data), .c_i(ram_out_S),.N(arg2), .rd_addr_data(rd_addr_data), .en_rd_data(rd_en_ram_data), .en_rd_S(rd_en_ram_S), .en_rd_N(rd_en_ram_N), .rd_addr_data_updated(rd_addr_data), .rd_addr_S(rd_addr_S), .rd_addr_N(rd_addr_N), .done_evp(done_out_evp), .result(result), .status(status));
 
EVB_FSM_3 #(.buffer_size(buffer_size))
		evb_command(.clk(clk), .rst(rst), .rst_instr(rst_instr), .start_evb(en_evb), .A(arg1), .b(arg2), .x_b(ram_out_data), .c_i(ram_out_S), .N(ram_out_N), .rd_addr_data(rd_addr_data), .done_evp(done_out_evp), .done_evb(done_out_evb), .en_rd_data(rd_en_ram_data), .en_rd_S(rd_en_ram_S), .en_rd_N(ed_en_ram_N), .rd_addr_data_updated(rd_addr_data), .rd_addr_S(rd_addr_S), .result(result), .status(status));
  
  
RST_FSM_3 #()
       rst_command(.clk(clk), .start_rst(en_rst), 
				.rst(rst_instr), .done_rst(done_out_rst));


always @(posedge clk or negedge rst)
begin
    if(!rst || !rst_instr)
    begin
        state_module <= STATE_START;
    end
    else
    begin
        state_module <= next_state_module;
    end
end

always @(state_module, start_fsm2, done_out_get_command, done_out_stp, done_out_evp, done_out_evb, done_out_rst, next_instr, instr)
begin
case(state_module)
    STATE_START:
    begin
        if(start_fsm2)
        begin
        case(next_instr)
            SETUP_INSTR: 
            begin
                next_state_module <= STATE_GET_COMMAND_START;
            end

            INSTR: 
            begin
                case(instr)//same mode signal that is passed to enable 
                    STP:
                    begin
                        next_state_module <= STATE_STP_START;
                    end

                    EVP:
                    begin
                        next_state_module <= STATE_EVP_START;
                    end

                    EVB: 
                    begin
                        next_state_module <= STATE_EVB_START;
                    end
                    RST: 
                    begin
                        next_state_module <= STATE_RST;
                    end
                    default:
                    begin
                        next_state_module <= STATE_START;
                    end

                endcase
            end
            default:
            begin
                next_state_module <= STATE_START;
            end
        endcase
        end
        else 
        begin
            next_state_module <= STATE_START;
        end
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
            next_state_module <= STATE_GET_COMMAND_FINISH;
        end
        else
        begin
            next_state_module <= STATE_GET_COMMAND_WAIT;
        end
    end
    STATE_GET_COMMAND_FINISH:
    begin
        next_state_module <= STATE_START;
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
            next_state_module <= STATE_OUTPUT;
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
            next_state_module <= STATE_OUTPUT;
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
            next_state_module <= STATE_OUTPUT;
        else if (done_out_evp)
			next_state_module <= STATE_EVB_OUTPUT;
		else
            next_state_module <= STATE_EVB_WAIT;
    end

	STATE_EVB_OUTPUT:
	begin
		if (done_out_evb)
			next_state_module <= STATE_OUTPUT;
		else if (done_out_evp)
			next_state_module <= STATE_EVB_OUTPUT;
		else
			next_state_module <= STATE_EVB_WAIT;
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
        next_state_module <= STATE_START;
    end

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
    STATE_GET_COMMAND_FINISH:
    begin
        en_wr_output_fifo <= 0;
        en_get_command <= 0;
        en_stp <= 0;
        en_evp <= 0;
        en_evb <= 0;
        en_rst <= 0;
        done_fsm2 <= 1;
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

	STATE_EVB_OUTPUT:
	begin
		en_wr_output_fifo <= 1;
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
        done_fsm2 <= 1;
    end

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
            
