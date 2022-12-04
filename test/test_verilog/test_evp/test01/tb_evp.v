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
	reg [2 : 0] N [7 : 0];
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

	// Test EVP computation for 3 + 4x + 2x^2 + x^3

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
	start_evp = 1;

	#2;

	/* STATE_RD_N: 1 */

	if (en_rd_N)
		N_i = N[rd_addr_N];
	else begin
		$display("1: en_rd_N is ", en_rd_N);
	end

	#2;

	/* STATE_CHECK_N: 2 */

	#2;

	/* STATE_RD_DATA: 3: 3 */

	if (en_rd_data)
		x = data[rd_addr_data];
	else begin
		$display("2: en_rd_data is ", en_rd_data);
	end

	//rd_addr_data = rd_addr_data_updated;

	if (en_rd_S)
		c_i = S[rd_addr_S];
	else begin
		$display("3: en_rd_S is ", en_rd_S);
	end

	#2;

	/* STATE_COMPUTE_SUM: 4 */

	#2;

	/* STATE_GET_NEXT_COEFF: 5: 2x */

	if (en_rd_S)
        c_i = S[rd_addr_S];
    else begin
        $display("4: en_rd_S is ", en_rd_S);
    end

	#2;

	/* STATE_COMPUTE_EXP: 6 */	

	#2;

	/* STATE_COMPUTE_SUM: 4 */

	#2;

	/* STATE_GET_NEXT_COEFF: 5: 4x^2 */

	if (en_rd_S)
        c_i = S[rd_addr_S];
    else begin
        $display("5: en_rd_S is ", en_rd_S);
    end

	#2;

	/* STATE_COMPUTE_EXP: 6 */

	#2;

	/* STATE_COMPUTE_SUM: 4 */

	#2;

	/* STATE_GET_NEXT_COEFF: 5: x^3 */

	if (en_rd_S)
        c_i = S[rd_addr_S];
    else begin
        $display("6: en_rd_S is ", en_rd_S);
    end

	#2;

	/* STATE_COMPUTE_EXP: 6 */

	#2;

	/* STATE_COMPUTE_SUM: 4 */

	#2;

	/* STATE_END: 8 */

	$display("result = ", result);
	$display("status = ", status);

	#2;

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
	
