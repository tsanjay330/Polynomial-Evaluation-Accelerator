`timescale 1ns/1ps

module tb_stp #(parameter word_size = 16, buffer_size = 1024)();
	reg clk;
	reg rst;
	reg rst_instr;
    reg [log2(buffer_size - 1) : 0] rd_addr_data;
	reg [log2(buffer_size - 1) : 0] rd_addr_N;
	reg [log2(buffer_size - 1) : 0] rd_addr_S;
	reg [2 : 0] A;
	reg [4 : 0] N;
    reg [15 : 0] c_i;
	reg [2:0] data [15:0];
	reg [7:0] S [15:0];
    wire done_stp;
	wire en_rd_data;
	wire en_rd_S;
	wire en_rd_N;
	reg  start_stp;
	wire [log2(buffer_size) - 1 : 0] rd_addr_data_updated;
	wire [log2(buffer_size) - 1 : 0] wr_addr_S;
	wire [log2(buffer_size) - 1 : 0] wr_addr_N;
	wire [15 : 0] c;
	wire [31 : 0] result;
	wire [31 : 0] status;
	

	integer i;
	integer descr;

	STP_FSM_3 #(.buffer_size(buffer_size))
		stp(.clk(clk), .rst(rst), .rst_instr(rst_instr), .start_stp(start_stp), .rd_addr_data(rd_addr_data), .A(A), .N(N), .next_c(c_i), .done_stp(done_stp), .en_rd_data(en_rd_data), .en_rd_S(en_rd_S), .en_rd_N(en_rd_N), .rd_addr_data_updated(rd_addr_data_updated), .wr_addr_S(wr_addr_S), .wr_addr_N(wr_addr_N), .c(c), .result(result), .status(status)); 

initial begin
	rst <= 0;
	#1;
	rst <= 1;
end	

initial begin
	for(i = 0; i < 200; i = i + 1) begin
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
		N[i] = 0;

	data[0] = 2;
	data[1] = 1;
	data[2] = 4;

	A = 0;
	rd_addr_data = 0;
	start_stp = 1;

	#2;

	/* STATE_COMPUTE0 */

	rd_addr_data = rd_addr_data_updated;
	
	if (en_rd_S)
		c_i = S[rd_addr_S];
	else begin
		$display("2: en_rd_S is low when it should be high");
		$finish;
	end

	if (en_rd_N)
		N = N[rd_addr_N];
	else begin
		$display("3: en_rd_N is low when it should be high");
		$finish;
	end

	#1;

	/* STATE_COMPUTE2 */		

	if (en_rd_S)
		c_i = S[rd_addr_S];
	else begin
		$display("4: en_rd_S is low when it should be high");
		$finish;
	end

	#1;

	/* STATE_COMPUTE1 */

	#2;

	/* STATE_COMPUTE2 */

	if (en_rd_S)
		c_i = S[rd_addr_S];
	else begin
		$display("5: en_rd_S is ", en_rd_S);
		$finish;
	end

	#1;

	/* STATE_COMPUTE1 */

	/*$display("a: en_rd_S is ", en_rd_S);

	#1

	$display("b: en_rd_S is ", en_rd_S);

	#1;

	$display("c: en_rd_S is ", en_rd_S);

	#1;

	$display("d: en_rd_S is ", en_rd_S);

	#1;

	$display("e: en_rd_S is ", en_rd_S);*/

	/* STATE_COMPUTE2 */

	if (en_rd_S)
		c_i = S[rd_addr_S];
	else begin
		$display("6: en_rd_S is low when it should be high");
		$finish;
	end

	#1;

	/* STATE_COMPUTE1 */

	$display("a: done_stp = ", done_stp);

	#1;

	$display("b: done_stp = ", done_stp);

	#1;

	$display("c: done_stp = ", done_stp);

	#1;

	$display("d: done_stp = ", done_stp);

	#1;

	$display("e: done_stp = ", done_stp);

	#1;

	$display("f: done_stp = ", done_stp);

	#1;

	$display("g: done_stp = ", done_stp);

	$display("result = ", result);
	$display("status = ", status);

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
	
