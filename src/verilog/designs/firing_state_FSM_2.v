//TODO:Add signals rd_en_command,data,S,N,x,c_i.
//TODO:Figure out number of states(whther to implement instr states in SETUP_COMP,COMP and OUTPUT
`timescale 1ns/1ps
module firing_state_FSM2
        #(parameter word_size = 16, buffer_size = 1024)//Not sure if we need buffer size for this one
        (input clk, rst,
        input [word_size - 1 : 0] data_in,
        input [word_size - 1 : 0] command_in,
        input [word_size - 1 : 0] ram_out_command;
        input start_fsm2,
        input [2 : 0] next_mode_in,
        output rd_in_data,
        output rd_in_command,
//        output reg [2 : 0] next_mode_out;
        output reg done_fsm2,
        output reg wr_out_result,
        output reg wr_out_status,
        output reg [word_size - 1 : 0] data_out_result,
        output reg [word_size - 1 : 0] data_out_status,
        output reg [2 : 0] A,
        output reg [4 : 0] N,
        output reg [15 : 0] c,
        output reg [15 : 0] SA,
        output reg [7 : 0] instr,
        output reg [2 : 0] arg1,
        output reg [4 : 0] arg2,
        output reg [1 : 0] error);
   
   localparam GET_COMMAND=3'b000, STP=3'b001; EVP=3'b010, EVB=3'b011, OUTPUT=3'b100, RST=3'b101;
   localparam STATE_START=4'b0000, STATE_GET_COMMAND_START=4'b0001, STATE_GET_COMMAND_WAIT=4'b0010, STATE_STP_START=4'b0011, STATE_STP_WAIT=4'b0100, STATE_EVP_START=4'b0101, STATE_EVP_WAIT=4'b0110, STATE_EVB_START=4'b0111, STATE_EVB_WAIT=4'b1000, STATE_OUTPUT=4'b1001, STATE_END=4'b1010;

   reg [2 : 0] state_module, next_state_module;

	/* Comment 1: these need to have clearer names. Suggestions commented below */
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
   reg en_mode_check_err;
   reg en_mode_wr_coeff;
   wire result, status;
   wire [2*word_size - 1 : 0] next_result_out, next_status_out;
   wire wr_en_ram_command, wr_en_ram_data, wr_en_ram_S, wr_en_ram_N;
   wire rd_en_ram_command, rd_en_ram_data, rd_en_ram_S, rd_en_ram_NN;
   wire ram_out_command, ram_out_data, ram_out_S, ram_out_N;
//   wire [2*word_size - 1 : 0] data_out_result, data_out_status;
/********************************
wire ram_out1, ram_out2;
wire wr_addr, rd_addr;
********************************/
/*single_port_ram #(.word_size(word_size), .buffer_size(buffer_size))
      RAM1(data_out_result, wr_addr, rd_addr, wr_en_ram, rd_en, clk, ram_out_result);  
single_port_ram #(.word_size(word_size), .buffer_size(buffer_size))
      RAM2(data_out_status, wr_addr, rd_addr, wr_en_ram, rd_en, clk, ram_out_status);
*/
/* Comment 2: RAM is not for result and status. RAM is for command, data, S, and N. Adding modified implementation in the comment below. */

/* Note 2: We may need separate variables for the rd_en and wr_en variables. This will depend upon how we ultimately implement RAM modules. At the moment, level-3 FSMs are handling enable signals, so they need to be separate. I've implemented this below. */

// Implementation 2:
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



/***************************************************
Instantiation of the nested FSM for get_command_FSM3
***************************************************/
get_command_FSM_3 #(.word_size(word_size))
          get_command(clk, rst, en_get_command, ram_out_command/*command_in*/, en_mode_check_err, N, en_rd_cmd, done_out_get_command, instr, arg1, error);

STP_FSM_3 #(.word_size(word_size))
       stp_command(clk, rst, en_stp, en_mode_wr_coeff, A, N, c, en_rd_data, done_out_stp, SA, result, status);     

EVP_FSM_3 #(.word_size(word_size))
       evp_command(clk, rst, en_evp, A, x, c_i, N, en_rd_data, en_rd_S, en_rd_N, done_out_evp, result, status);

EVB_FSM_3 #(.word_size(word_size))
       evb_command(clk, rst, en_evb, A, b, x_b, c_i, N, done_out_evb, en_rd_data, en_rd_S, en_rd_N, result, status);

RST_FSM_3 #(.word_size(word_size))
       rst_command(clk, rst, en_rst, done_out_rst);

/* Note 3: based on (2), might need to change some inputs here. ex. I think command_in in STP instantiation should be replaced with ram_out_command */

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
        GET_COMMAND:
        begin
            next_state_module <= STATE_GET_COMMAND_START;
        end
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
        OUTPUT:
        begin
            next_state_module <= STATE_OUTPUT;
        end
        default:
        begin
            next_state_module <= STATE_START;
        end
        endcase
    end

	/* Comment 4: can we change this to a case statement? It's cleaner and matches the formatting of the rest of the modules */

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

/* Comment 5: we briefly talked about this before; didn't we want to make one single wr_out_output? Since result and status are always outputted together? If we don't do this, then you need to be setting wr_out_status in all of these */

/* Comment 6: I'm confused by some of your wires and regs. It seems you are expecting one single output value from get_command, but get_command outputs instr, arg1, arg2, result, and status (and done signal). Is data_out supposed to be for result? We need to be setting values for all the outputs, and that also includes status. Apply this same logic to the rest of the states too. */

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
            
