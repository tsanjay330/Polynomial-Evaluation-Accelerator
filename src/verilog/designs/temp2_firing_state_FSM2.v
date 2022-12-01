//TODO:Add signals rd_en_command,data,S,N,x,c_i
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
module temp2_firing_state_FSM2
        #(parameter word_size = 16, buffer_size = 1024)
        (input clk, rst,
        input [word_size - 1 : 0] data_in,
        input [word_size - 1 : 0] command_in,
        input start_fsm2,
        input [1 : 0] next_mode_in,
		output reg [7:0] instr,
        output rd_in_data,
        output rd_in_command,
        output reg done_fsm2,
        output reg wr_out_result,
        output reg wr_out_status,
        output reg [word_size - 1 : 0] data_out_result,
        output reg [word_size - 1 : 0] data_out_status);
   
	localparam SETUP_COMP = 2'b00, COMP = 2'b01, OUTPUT = 2'b10;

	localparam GET_COMMAND=3'b000, STP=3'b001, EVP=3'b010, 
		EVB=3'b011, OUTPUT=3'b100, RST=3'b101;

	localparam STATE_START=4'b0000, STATE_GET_COMMAND_START=4'b0001, 
		STATE_GET_COMMAND_WAIT=4'b0010, STATE_STP_START=4'b0011, 
		STATE_STP_WAIT=4'b0100, STATE_EVP_START=4'b0101, 
		STATE_EVP_WAIT=4'b0110, STATE_EVB_START=4'b0111, 
		STATE_EVB_WAIT=4'b1000, STATE_OUTPUT=4'b1001, STATE_END=4'b1010;

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
   wire en_mode_check_err;
   wire en_mode_wr_coeff;
   //wire [7:0] instr;
   wire [2:0] arg1;
   wire [4:0] arg2; 
   wire [1:0] err_out;
   wire [2:0] A;
   wire [4:0] b;
   wire [word_size - 1 : 0] x_b;
   wire [4:0] N;
   wire [word_size -1 : 0] c;
   wire [word_size -1 : 0] next_c;
   wire [word_size - 1 : 0] c_i;
   wire en_rd_cmd;
   wire [log2(buffer_size) - 1 : 0] rd_addr_data, rd_addr_data_updated;
   wire [7:0] rd_addr_S;
   wire [log2(buffer_size) - 1 : 0] wr_addr_S;
   wire [2*word_size - 1 : 0] result, status;
   wire [2*word_size - 1 : 0] next_result_out, next_status_out;
   wire rst;
   wire [log2(buffer_size) - 1 : 0] wr_en_ram_command, wr_en_ram_data, wr_en_ram_S, wr_en_ram_N;
   wire [log2(buffer_size) - 1 : 0] rd_en_ram_command, rd_en_ram_data, rd_en_ram_S, rd_en_ram_NN;
   wire [word_size - 1 : 0] ram_out_command, ram_out_data, ram_out_S, ram_out_N;
/****************************************
Regs and wires used in RAM modules
****************************************/
   

/****************************************************************
Instantiation of RAM modules
****************************************************************/
single_port_ram #(.word_size(word_size), .buffer_size(buffer_size))
	RAM_COMMAND(command_in, wr_addr, rd_addr, wr_en_ram_command, 
	rd_en_ram_command, clk, ram_out_command);

single_port_ram #(.word_size(word_size), .buffer_size(buffer_size))
	RAM_DATA(data_in, wr_addr, rd_addr, wr_en_ram_data, rd_en_ram_data,
	clk, ram_out_data);

single_port_ram #(.word_size(word_size), .buffer_size(buffer_size))
	RAM_S(coeff, wr_addr, rd_addr, wr_en_ram_S, rd_en_ram_S, clk, ram_out_S);

single_port_ram #(.word_size(word_size), .buffer_size(buffer_size))
	RAM_N((what should this be? arg2, which is sometimes N?), wr_addr, rd_addr,
			wr_en_ram_N, rd_en_ram_NN, clk, ram_out_N);



/***********************************************************************
Instantiation of the nested FSM for get_command_FSM3, STP, EVP, EVB, RST
***********************************************************************/
get_command_FSM_3 #()
          get_command(.clk(clk), .rst(rst), .en_get_command(start_get_cmd), .ram_out_command(command_in), .en_mode_check_err(en_mode_check_err), .en_rd_cmd(en_rd_cmd), .done_out_get_command(done_get_cmd), /*(.instr(instr))*/, .arg1(arg1), .arg2(arg2), .error_out(error));

STP_FSM_3 #(.buffer_size(buffer_size))
       stp_command(.clk(clk), .rst(rst), .en_stp(start_stp), .rd_addr_data(rd_addr_data), .A(A), .N(N), .next_c(next_c), .done_out_stp(done_stp), .rd_en_ram_data(en_rd_data), .wr_en_ram_S(en_wr_S), .rd_addr_data_updated(rd_addr_data_updated), .wr_addr_S(wr_addr_S), .c(c), .result(result), .status(status));     

EVP_FSM_3 #(.buffer_size(buffer_size))
       evp_command(.clk(clk), .rst(rst), .en_evp(start_evp),.(A)A, .x(x), c_i, .N(N), .rd_addr_data(rd_addr_data), .rd_en_ram_data(en_rd_data), .rd_en_ram_S(en_rd_S), .rd_en_ram_N(en_rd_N), .rd_addr_data_updated(rd_addr_data_updated), .rd_addr_S(rd_addr_S), .done_out_evp(done_evp), .result(result), .status(status));

EVB_FSM_3 #(.buffer_size(buffer_size))
       evb_command(.clk(clk), .rst(rst), .en_evb(start_evb), .A(A), .b(b), .x_b(x_b), .c_i(c_i), .N(N), .rd_addr_data(rd_addr_data), .done_out_evb(done_evb), .rd_en_ram_data(en_rd_data), .rd_en_ram_S(en_rd_S), .rd_en_ram_N(en_rd_N), .rd_addr_data_updated(rd_addr_data_updated), .rd_addr_S(rd_addr_S), .result(result), .status(status));

RST_FSM_3 #()
       rst_command(.clk(clk), .en_rst(start_rst), .rst(rst), .done_out_rst(done_rst));


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
            SETUP_COMP: begin
                next_state_module <= STATE_GET_COMMAND_START;
            end

            COMP: begin
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
            next_state_module <= STATE_END;
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
            next_state_module <= STATE_END;
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
            next_state_module <= STATE_END;
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
            next_state_module <= STATE_END;
        end
        else
        begin
            next_state_module <= STATE_EVB_WAIT;
        end
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
    default:
    begin
        next_state_module <= STATE_START;
    end
    endcase
end

/**************************************
OUTPUT SIGNALS
**************************************/
always @(state_module, next_result_out, next_status_out)
begin
    case(state_module)
    STATE_START:
    begin
        wr_out_result <= 0;
        wr_out_status <= 0;
        en_get_command <= 0;
        en_stp <= 0;
        en_evp <= 0;
        en_evb <= 0;
        en_rst <= 0;
        done_fsm2 <= 0;
        data_out_result <= next_result_out;
        data_out_status <= next_status_out;
    end

    STATE_GET_COMMAND_START:
    begin
        wr_out_result <= 0;
        wr_out_status <= 0;
        en_get_command <= 1;
        en_stp <= 0;
        en_evp <= 0;
        en_evb <= 0;
        en_rst <= 0;
        done_fsm2 <= 0;
        data_out_result <= next_result_out;
        data_out_status <= next_status_out;
    end

    STATE_GET_COMMAND_WAIT:
    begin
        wr_out_result <= 0;
        wr_out_status <= 0;
        en_get_command <= 0;
        en_stp <= 0;
        en_evp <= 0;
        en_evb <= 0;
        en_rst <= 0;
        done_fsm2 <= 0;
        data_out_result <= next_result_out;
        data_out_status <= next_status_out;
    end
    STATE_STP_START:
    begin
        wr_out_result <= 0;
        wr_out_status <= 0;
        en_get_command <= 0;
        en_stp <= 1;
        en_evp <= 0;
        en_evb <= 0;
        en_rst <= 0;
        done_fsm2 <= 0;
        data_out_result <= next_result_out;
        data_out_status <= next_status_out;
    end
    STATE_STP_WAIT:
    begin
        wr_out_result <= 0;
        wr_out_status <= 0;
        en_get_command <= 0;
        en_stp <= 0;
        en_evp <= 0;
        en_evb <= 0;
        en_rst <= 0;
        done_fsm2 <= 0;
        data_out_result <= next_result_out;
        data_out_status <= next_status_out;
    end
    STATE_EVP_START:
    begin
        wr_out_result <= 0;
        wr_out_status <= 0;
        en_get_command <= 0;
        en_stp <= 0;
        en_evp <= 1;
        en_evb <= 0;
        en_rst <= 0;
        done_fsm2 <= 0;
        data_out_result <= next_result_out;
        data_out_status <= next_status_out;
    end
    STATE_EVP_WAIT:
    begin
        wr_out_result <= 0;
        wr_out_status <= 0;
        en_get_command <= 0;
        en_stp <= 0;
        en_evp <= 0;
        en_evb <= 0;
        en_rst <= 0;
        done_fsm2 <= 0;
        data_out_result <= next_result_out;
        data_out_status <= next_status_out;
    end
    STATE_EVB_START:
    begin
        wr_out_result <= 0;
        wr_out_status <= 0;
        en_get_command <= 0;
        en_stp <= 0;
        en_evp <= 0;
        en_evb <= 1;
        en_rst <= 0;
        done_fsm2 <= 0;
        data_out_result <= next_result_out;
        data_out_status <= next_status_out;
    end
    STATE_EVB_WAIT:
    begin
        wr_out_result <= 0;
        wr_out_status <= 0;
        en_get_command <= 0;
        en_stp <= 0;
        en_evp <= 0;
        en_evb <= 0;
        en_rst <= 0;
        done_fsm2 <= 0;
        data_out_result <= next_result_out;
        data_out_status <= next_status_out;
    end

    STATE_OUTPUT:
    begin
        wr_out_result <= 1;
        wr_out_status <= 1;
        en_get_command <= 0;
        en_stp <= 0;
        en_evp <= 0;
        en_evb <= 0;
        en_rst <= 0;
        done_fsm2 <= 0;
        data_out_result <= next_result_out;
        data_out_status <= next_status_out;
    end

    STATE_END:
    begin
        wr_out_result <= 0;
        wr_out_status <= 0;
        en_get_command <= 0;
        en_stp <= 0;
        en_evp <= 0;
        en_evb <= 0;
        en_rst <= 0;
        done_fsm2 <= 1;
        data_out_result <= next_result_out;
        data_out_status <= next_status_out;
    end
    default:
    begin
        wr_out_result <= 0;
        wr_out_status <= 0;
        en_get_command <= 0;
        en_stp <= 0;
        en_evp <= 0;
        en_evb <= 0;
        en_rst <= 0;
        done_fsm2 <= 0;
        data_out_result <= next_result_out;
        data_out_status <= next_status_out;
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
            
