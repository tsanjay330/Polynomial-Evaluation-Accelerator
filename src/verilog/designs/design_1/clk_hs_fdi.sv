module gen_chiplet_fdi_clk_hs (
	//Global signals
	input clk_i,
	input rstn_i,
	input swrst_i,
	//Start and done signals
	input start_hs_i,
	output done_hs_o,
	//handshake signals
	input clk_hs_done_i,
	input wake_hs_done_i,
	//rx-active handshake signals
	input rx_active_hs_done_i,
	input rx_active_hs_stop_i
); 

	reg req_instr;
	reg req_next_instr;

	wire clk_start_i;
	wire clk_done_o;
	wire wake_ready_i;
	wire wake_alert_o;
	wire clk_req_o;
	wire clk_ack_i;
	wire wake_req_i;
	wire wake_ack_o;
//handshake module instantiations for rx active
	wire ready;
	wire done;
	wire stop;
	wire rx_active_req;
	wire rx_active_sts;
	 bit [1:0] START_HS = 2'b00;
	localparam bit [1:0] DONE_HS = 2'b01;
	localparam bit [1:0] STOP = 2'b10;


	gen_cm_ucie_hs_us hs_us (
		.clk_i(clk_i),
		.rstn_i(rstn_i),
		.swrst_i(swrst_i),

		.clk_start_i(clk_hs_start_o),
		.clk_done_o(clk_hs_done_i),
		.wake_ready_i(wake_ready_i),
		.wake_alert_o(wake_alert_o),
		.stall_start_i(1'b0),
		.stall_done_o(1'b0),

		.clk_req_o(clk_req_o),
		.clk_ack_i(clk_ack_i),
		.wake_req_i(wake_req_i),
		.wake_ack_o(wake_ack_o),
		.stall_req_o(1'b0),
		.stall_ack_i(1'b0)
	);

	cm_ucie_d2d_rx_rxactive hs_fdi (
		.clk_i(clk_i),
		.rstn_i(rstn_i),
		.swrst_i(swrst_i),

		.start_i(rx_active_hs_start_o),
		.stop_i(rx_active_hs_stop_i),
		.done_o(rx_active_hs_done_i),

		.hs_clk_start_o(clk_done_o),
		.hs_clk_done_i(clk_start_o),

		.req_o(rx_active_req),
		.sts_i(rx_active_sts)
	);


	always_ff @(posedge clk_i) begin : proc_req_instr
		if(~rstn_i) begin
			req_instr <= START_READY;
		end else begin
			req_instr <= (swrst_i) ? STATE_READY : req_next_instr;
		end
	end

	assign 
