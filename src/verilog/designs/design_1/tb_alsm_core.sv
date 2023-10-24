`timescale 1ns/10ps

module tb_cm_ucie_d2d_rx_alsm_core ();
    logic clk;
    logic rstn;
    logic swrst;
    logic [3:0] rdi_state_sts;
    logic [3:0] state_sts;
    logic bringup_done;
    logic bringup_start;

    localparam bit [3:0] FDI_ACTIVE = 4'b0001;
    localparam bit [3:0] RDI_RESET  = 4'b0000;
    localparam bit [3:0] RDI_ACTIVE = 4'b0001;

    cm_ucie_d2d_rx_alsm_core dut (
        .clk_i(clk),
        .rstn_i(rstn),
        .swrst_i(swrst),

        .rdi_state_sts_i(rdi_state_sts),
        .state_sts_o(state_sts),

        .bringup_done_i(bringup_done),
        .bringup_start_o(bringup_start)
    );

    task automatic wait_start();
        @(posedge clk);
        while (~rstn) begin
            @(posedge clk);
        end
    endtask

    initial begin
`ifdef DUMP_VCD
        $dumpfile("waves.vcd");
        $dumpvars(0, tb_cm_ucie_d2d_rx_alsm_core);
        $dumpon;
`elsif DUMP_FSDB
        $fsdbDumpfile("waves.fsdb");
        $fsdbDumpvars(0, tb_cm_ucie_d2d_rx_alsm_core, "+all");
        $fsdbDumpon;
`endif
    end

    initial begin
        clk = 1;
        forever #0.5 clk = ~clk;
    end

    initial begin
        swrst = 0;
        rstn = 0;
        #10;
        rstn = 1;
        $display("Reset disabled @ %0t", $realtime);
    end

    initial begin
        #1000 $finish();
    end

    initial begin
        bringup_done = 0;

        wait_start();

        while (!bringup_start) begin
            @(posedge clk);
        end
        repeat ($urandom_range(25, 1)) begin
            @(posedge clk);
        end
        bringup_done = 1;
        @(posedge clk);
        bringup_done = 0;
    end

    initial begin
        rdi_state_sts = RDI_RESET;

        wait_start();

        @(posedge clk);
        #0 rdi_state_sts = RDI_ACTIVE; /* avoid race condition */
        $display("RDI is ACTIVE @ %0t", $realtime);

        while (state_sts != FDI_ACTIVE) begin
            @(posedge clk);
        end
        $display("FDI is ACTIVE @ %0t", $realtime);

        @(posedge clk);
        $finish();
    end
endmodule
