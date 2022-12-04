`timescale 1ns/1ps

module tb_evp
		#(parameter word_size = 16, buffer_size = 1024)();

	reg clk;
	reg rst;

	reg rst_instr;
	reg start_evp;
	reg [2 : 0] A;
	reg [15 : 0] x;
	reg [15 : 0] c_i;
	reg [4 : 0] N_i;
	reg [log2(buffer_size)-1 : 0] rd_addr_data;
	wire en_rd_data;
	wire en_rd_S;
	wire en_rd_N;
	wire [log2(buffer_size)-1 : 0] rd_addr_data_updated;
	wire [6 : 0] rd_addr_S;
	wire [2 : 0] rd_addr_N;
	wire done_evp;
	wire [31 : 0] result;
	wire [31 : 0] status;

	reg [15 : 0] S [87 : 0];
	reg [4 : 0] N [7 : 0];
	reg [31 : 0] data [2 : 0];

	integer i;
	integer descr;

	EVP_FSM_3 #(.buffer_size(buffer_size))
		evp1(.clk(clk), .rst(rst), .rst_instr(rst_instr), .start_evp(start_evp),
			.A(A), .x(x), .c_i(c_i), .N(N_i), .rd_addr_data(rd_addr_data), 
			.en_rd_data(en_rd_data), .en_rd_S(en_rd_S), .en_rd_N(en_rd_N), 
			.rd_addr_data_updated(rd_addr_data_updated), .rd_addr_S(rd_addr_S),
			.rd_addr_N(rd_addr_N), .done_evp(done_evp), .result(result), 
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

	for (i = 0; i < 11; i = i + 1)
		S[i] = 0;

	S[11] = 16'd25432;

	for (i = 12; i < 88; i = i + 1)
		S[i] = 0;

	N[0] = 5'b11111;
	N[1] = 5'd0;
	
	for (i = 2; i < 8; i = i + 1)
		N[i] = 5'b11111;

	data[0] = 1;
	data[1] = 2;
	data[2] = 4;


	/****** Test EVP 1 ******/

	$display("Test 1");

	A = 1;
	rd_addr_data = 1;
	start_evp = 1;

	/* STATE_START */

	#2;

	start_evp = 0;

	/* STATE_RD_N: 1 */

	if (en_rd_N)
		N_i = N[rd_addr_N];
	else 
		$display("1: en_rd_N is ", en_rd_N);

	#2;

	/* STATE_CHECK_N: 2 */

	#2;

	/* STATE_RD_DATA: 3: 3 */

	if (en_rd_data)
		x = data[rd_addr_data];
	else 
		$display("2: en_rd_data is ", en_rd_data);

	if (en_rd_S)
		c_i = S[rd_addr_S];
	else
		$display("3: en_rd_S is ", en_rd_S);

	#2;

	/* STATE_COMPUTE_SUM */
    
	#2;

	/* STATE_OUTPUT */

	#2;

	/* STATE_END */

	if (done_evp) begin
		$display("result = ", result);
		$display("status = ", status);
	end


	/****** Test 2 ******/

	$display("Test 2: using rst_instr");
	rst_instr = 0;
	
	#2;

	$display("rd_addr_data is ", rd_addr_data_updated);

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
	
