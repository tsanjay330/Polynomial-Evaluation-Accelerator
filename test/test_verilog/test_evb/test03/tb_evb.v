`timescale 1ns/1ps

module tb_evb
		#(parameter word_size = 16, buffer_size = 1024)();

	reg clk;
	reg rst;

	reg rst_instr;
	reg start_evb;
	reg [2 : 0] A;
	reg [4 : 0] b;
	reg [15 : 0] x_b;
	reg [15 : 0] c_i;
	reg [4 : 0] N_i;
	reg [log2(buffer_size)-1 : 0] rd_addr_data;
	wire done_evp;
	wire done_evb;
	wire en_rd_data;
	wire en_rd_S;
	wire en_rd_N;
	wire [log2(buffer_size)-1 : 0] rd_addr_data_updated;
	wire [6 : 0] rd_addr_S;
	wire [2 : 0] rd_addr_N;
	wire [31 : 0] result;
	wire [31 : 0] status;

	reg [15 : 0] S [87 : 0];
	reg [4 : 0] N [7 : 0];
	reg [31 : 0] data [2 : 0];

	integer i;
	integer descr;

	EVB_FSM_3 #(.buffer_size(buffer_size))
		evb1(.clk(clk), .rst(rst), .rst_instr(rst_instr), .start_evb(start_evb),
			.A(A), .b(b), .x_b(x_b), .c_i(c_i), .N(N_i), 
			.rd_addr_data(rd_addr_data), .done_evp(done_evp), 
			.done_evb(done_evb), .en_rd_data(en_rd_data), .en_rd_S(en_rd_S),
			.en_rd_N(en_rd_N), .rd_addr_data_updated(rd_addr_data_updated),
			.rd_addr_S(rd_addr_S), .rd_addr_N(rd_addr_N), .result(result), 
			.status(status));

initial begin
	rst <= 0;
	#1;
	rst <= 1;
end	

initial begin
	for(i = 0; i < 1000; i = i + 1) begin
		#1 clk <= 1;
		#1 clk <= 0;
	end
end

initial begin
	descr = $fopen("out.txt");

	rst_instr = 1;

	S[0] = 16'd3;
	S[1] = 16'd4;
	S[2] = 16'd2;
	S[3] = 16'd1;

	for (i = 4; i < 88; i = i + 1)
		S[i] = 0;

	N[0] = 3'd3;
	
	for (i = 1; i < 8; i = i + 1)
		N[i] = 5'b11111;

	data[0] = 1;
	data[1] = 2;
	data[2] = 3;

	// Test EVB 5 when STP 5 has not yet been done

	A = 5;
	b = 3;
	rd_addr_data = 0;
	start_evb = 1;

	$display("1st EVP");

	/* STATE_START */

	#2;

	/* STATE_FIRST_EVP_EN_EVP */

	start_evb = 0;

	/* EVP: STATE_START */

	#2;

    /* EVP: STATE_RD_N: 1 */

    if (en_rd_N)
        N_i = N[rd_addr_N];
    else
        $display("1: en_rd_N is ", en_rd_N);

    #2;

    /* EVP: STATE_CHECK_N: 2 */

    #2;

	/* EVP: STATE_ERROR */

	#2;

	/* EVP: STATE_END */

	/* STATE_FIRST_EVP_DONE */

	#2;

	if (done_evp) begin
		$display("result = ", result);
		$display("status = ", status);
	end

	/* STATE_CHECK_B */

	#2;

	$display("2nd EVP");

	/* STATE_EVP_EN_EVP */

	#2;

	/* STATE_EVP_WAIT */

	/* EVP: STATE_START */

    #2;

    /* EVP: STATE_RD_N: 1 */

    if (en_rd_N)
        N_i = N[rd_addr_N];
    else begin
        $display("1: en_rd_N is ", en_rd_N);
    end

    #2;

    /* EVP: STATE_CHECK_N: 2 */

    #2;

    /* EVP: STATE_ERROR */

    #2;

    /* EVP: STATE_END */

	/* STATE_EVP_DONE */

    #2;

	/* STATE_CHECK_B */
	
	if (done_evp) begin
		$display("result = ", result);
		$display("status = ", status);
	end

	#2;

	/* STATE_EVP_EN_EVP */

	#2;

	/* STATE_EVP_WAIT */

	$display("3rd EVP");

	/* EVP: STATE_START */

    #2;

    /* EVP: STATE_RD_N: 1 */

    if (en_rd_N)
        N_i = N[rd_addr_N];
    else begin
        $display("1: en_rd_N is ", en_rd_N);
    end

    #2;

    /* EVP: STATE_CHECK_N: 2 */

    #2;

    /* EVP: STATE_ERROR */

    #2;

    /* EVP: STATE_END */

	/* STATE_EVP_DONE */

    #2;

    if (done_evp) begin
        $display("result = ", result);
        $display("status = ", status);
    end

	/* STATE_CHECK_B */

    #2;

	/* STATE_END */

	#2;

	if (done_evb)
		$display("done_evb");

	$finish;

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
endfunction

endmodule
	
