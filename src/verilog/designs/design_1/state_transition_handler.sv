module xyz(
  input start_fsm;
  output done_fsm;
);
  reg [3:0] state_module;
  reg [3:0] next_state_module;
  reg state_change;
  reg [3:0] op;
  reg [3:0] next_op;
  reg op_change;

  wire state_error;
  wire op_error;
  wire abort;
  wire success;

  always @(posedge clk)
    begin
      if(start_fsm)
        begin
          if(state_error)
            begin





              
              
  
  
always_comb begin                                                                                                 |    localparam bit [2:0] CAL    = 3'b010;                                                                             
        if (abort) begin                                                                                              |    localparam bit [2:0] REPCLK = 3'b011;                                                                             
            next_op = INIT;                                                                                           |    localparam bit [2:0] REPVLD = 3'b100;                                                                             
        end else if (op_change) begin                                                                                 |    localparam bit [2:0] REVMB  = 3'b101;                                                                             
            case (op)                                                                                                 |    localparam bit [2:0] REPMB  = 3'b110;                                                                             
            INIT: begin                                                                                               |                                                                                                                      
                case (state)                                                                                          |    localparam bit [2:0] INIT   = 3'b000;                                                                             
                CAL: next_op = DONE;                                                                                  |    localparam bit [2:0] RUN    = 3'b001;                                                                             
                REVMB: next_op = CLRERR;                                                                              |    localparam bit [2:0] RESULT = 3'b010;                                                                             
                default: next_op = RUN;                                                                               |    localparam bit [2:0] DONE   = 3'b011;                                                                             
                endcase                                                                                               |    localparam bit [2:0] CLRERR = 3'b100;                                                                             
            end                                                                                                       |    localparam bit [2:0] APPLY  = 3'b101;                                                                             
            RUN: next_op = s_repmb ? APPLY : RESULT;                                                                  |    localparam bit [2:0] RERUN  = 3'b110;                                                                             
            RESULT: next_op = s_revmb && !success ? CLRERR : DONE;                                                    |                                                                                                                      
            CLRERR: next_op = RUN;                                                                                    |    reg mx_start;                                                                                                     
            APPLY: next_op = success ? DONE : RERUN;                                                                  |    wire mx_done;                                                                                                     
            RERUN: next_op = DONE;                                                                                    |    reg mx_done_d;                                                                                                    
            default: next_op = INIT;                                                                                  |                                                                                                                      
            endcase                                                                                                   |    reg detect;                                                                                                       
        end else begin                                                                                                |    reg detect_d;                                                                                                     
            next_op = op;                                                                                             |                                                                                                                      
        end                                                                                                           |    reg train;                                                                                                        
    end  








              assign o_init = (op == INIT) ? 1'b1 : 1'b0;
    assign o_run = (op == RUN) ? 1'b1 : 1'b0;
    assign o_result = (op == RESULT) ? 1'b1 : 1'b0;
    assign o_done = (op == DONE) ? 1'b1 : 1'b0;
    assign o_clrerr = (op == CLRERR) ? 1'b1 : 1'b0;
    assign o_apply = (op == APPLY) ? 1'b1 : 1'b0;
    assign o_rerun = (op == RERUN) ? 1'b1 : 1'b0;
    assign o_result_end = o_result & mx_done;
    assign o_apply_end = o_apply & mx_done;

    always_comb begin
        case (state)
        REPCLK,
        REPVLD,
        REVMB: train_done = train_done_i;
        REPMB: train_done = pt_done_i;
        default: train_done = 1'b0;
        endcase
    end

    assign success = tx_pass & rx_pass;
    assign trained = trained_d | train_done;
    assign abort = (s_check & o_result_end & ~success)
                   | (s_repmb & o_apply_end & wdeg_error_i)
                   | ping_8ms_i;

    always_ff @(posedge clk_i) begin
        if (!rstn_i || swrst_i) begin
            retry <= 1'b0;
            trained_d <= 1'b0;
        end else begin
            retry <= (o_result_end | retry) & ~state_change;
            trained_d <= trained & ~(state_change | op_change);
        end
    end

    /* message exchange outputs */
    always_comb begin
        case (state)
        IDLE: mx_start = state_change;
        PARAM: mx_start = 1'b0;
        REPCLK,
        REPVLD: mx_start = ~o_init & op_change;
        REPMB: mx_start = ~o_init & ~(o_apply & ~success) & op_change & ~state_change;
        default: mx_start = op_change;
        endcase
    end

    always_ff @(posedge clk_i) begin
        if (!rstn_i) begin
            mx_start_o <= 1'b0;
        end else begin
            mx_start_o <= mx_start;
        end
    end

    assign mx_ready_o = 1'b1;
    assign mx_done = mx_done_d | mx_done_i;

    always_ff @(posedge clk_i) begin
        if (!rstn_i) begin
            mx_done_d <= 1'b0;
        end else begin
            mx_done_d <= mx_done & ~(state_change | op_change);
        end
    end

    assign mx_req_o = (s_param & req_par_cfg_i)
                      | (s_cal & req_cal_done_i)
                      | (s_repclk & ((o_init & req_rclk_init_i)
                                     | ((o_run | o_result) & req_rclk_res_i)
                                     | (o_done & req_rclk_done_i)))
                      | (s_repvld & ((o_init & req_rvld_init_i)
                                     | ((o_run | o_result) & req_rvld_res_i)
                                     | (o_done & req_rvld_done_i)))
                      | (s_revmb & ((o_init & req_rev_init_i)
                                    | (o_clrerr & req_rev_clrerr_i)
                                    | ((o_run | o_result) & req_rev_res_i)
                                    | (o_done & req_rev_done_i)))
                      | (s_repmb & ((o_init & req_rep_start_i)
                                    | (o_apply & req_rep_degr_i)
                                    | (o_done & req_rep_end_i)));

    assign mx_rsp_o = (s_param & rsp_par_cfg_i)
                      | (s_cal & rsp_cal_done_i)
                      | (s_repclk & ((o_init & rsp_rclk_init_i)
                                     | ((o_run | o_result) & rsp_rclk_res_i)
                                     | (o_done & rsp_rclk_done_i)))
                      | (s_repvld & ((o_init & rsp_rvld_init_i)
                                     | ((o_run | o_result) & rsp_rvld_res_i)
                                     | (o_done & rsp_rvld_done_i)))
                      | (s_revmb & ((o_init & rsp_rev_init_i)
                                    | (o_clrerr & rsp_rev_clrerr_i)
                                    | ((o_run | o_result) & rsp_rev_res_i)
                                    | (o_done & rsp_rev_done_i)))
                      | (s_repmb & ((o_init & rsp_rep_start_i)
                                    | (o_apply & rsp_rep_degr_i)
                                    | (o_done & rsp_rep_end_i)));

    /* sideband message transmission outputs */
    assign req_par_cfg_o = s_param & mx_req_i;
    assign req_cal_done_o = s_cal & o_done & mx_req_i;
    assign req_rclk_init_o = s_repclk & o_init & mx_req_i;
    assign req_rclk_res_o = s_repclk & o_result & mx_req_i;
    assign req_rclk_done_o = s_repclk & o_done & mx_req_i;
    assign req_rclk_apply_o = 1'b0;
    assign req_rclk_chkrep_o = 1'b0;
    assign req_rclk_chkres_o = 1'b0;
    assign req_rvld_init_o = s_repvld & o_init & mx_req_i;
    assign req_rvld_res_o = s_repvld & o_result & mx_req_i;
    assign req_rvld_done_o = s_repvld & o_done & mx_req_i;
    assign req_rvld_apply_o = 1'b0;
    assign req_rev_init_o = s_revmb & o_init & mx_req_i;
    assign req_rev_clrerr_o = s_revmb & o_clrerr & mx_req_i;
    assign req_rev_res_o = s_revmb & o_result & mx_req_i;
    assign req_rev_done_o = s_revmb & o_done & mx_req_i;
    assign req_rep_start_o = s_repmb & o_init & mx_req_i;
    assign req_rep_end_o = s_repmb & o_done & mx_req_i;
    assign req_rep_degr_o = s_repmb & o_apply & mx_req_i;
    assign req_rep_apply_o = 1'b0;

    assign rsp_par_cfg_o = s_param & mx_rsp_i;
    assign rsp_cal_done_o = s_cal & o_done & mx_rsp_i;
    assign rsp_rclk_init_o = s_repclk & o_init & mx_rsp_i;
    assign rsp_rclk_res_o = s_repclk & o_result & mx_rsp_i;
    assign rsp_rclk_done_o = s_repclk & o_done & mx_rsp_i;
    assign rsp_rclk_apply_o = 1'b0;
    assign rsp_rclk_chkrep_o = 1'b0;
    assign rsp_rclk_chkres_o = 1'b0;
    assign rsp_rvld_init_o = s_repvld & o_init & mx_rsp_i;
    assign rsp_rvld_res_o = s_repvld & o_result & mx_rsp_i;
    assign rsp_rvld_done_o = s_repvld & o_done & mx_rsp_i;
    assign rsp_rvld_apply_o = 1'b0;
    assign rsp_rev_init_o = s_revmb & o_init & mx_rsp_i;
    assign rsp_rev_clrerr_o = s_revmb & o_clrerr & mx_rsp_i;
    assign rsp_rev_res_o = s_revmb & o_result & mx_rsp_i;
    assign rsp_rev_done_o = s_revmb & o_done & mx_rsp_i;
    assign rsp_rep_start_o = s_repmb & o_init & mx_rsp_i;
    assign rsp_rep_end_o = s_repmb & o_done & mx_rsp_i;
    assign rsp_rep_degr_o = s_repmb & o_apply & mx_rsp_i;
    assign rsp_rep_apply_o = 1'b0;

    /* transmitter */
    always_comb begin
        case (state)
        REPCLK: train = (rsp_rclk_init_i | train_d) & ~train_done_i;
        REPVLD: train = (rsp_rvld_init_i | train_d) & ~train_done_i;
        REVMB: train = (rsp_rev_clrerr_i | train_d) & ~train_done_i;
        default: train = 1'b0;
        endcase
    end

    always_ff @(posedge clk_i) begin
        if (!rstn_i) begin
            train_d <= 1'b0;
        end else begin
            train_d <= train;
        end
    end

    assign train_clk_o = s_repclk;
    assign train_dat_o = s_revmb;
    assign train_vld_o = s_repvld;
    assign train_start_o = train_d;

    /* receiver */
    always_comb begin
        case (state)
        REPCLK: detect = (req_rclk_init_i | detect_d) & ~req_rclk_res_i;
        REPVLD: detect = (req_rvld_init_i | detect_d) & ~req_rvld_res_i;
        REVMB: detect = (req_rev_clrerr_i | detect_d) & ~req_rev_res_i;
        default: detect = 1'b0;
        endcase
    end

    always_ff @(posedge clk_i) begin
        if (!rstn_i) begin
            detect_d <= 1'b0;
        end else begin
            detect_d <= detect;
        end
    end

    assign detect_clk_o = s_repclk & detect_d;
    assign detect_dat_o = s_revmb & detect_d;
    assign detect_vld_o = s_repvld & detect_d;

    /* passing signal */
    always_comb begin
        case (state)
        CAL: rx_pass_sel = serdes_rx_cal_done_i;
        REPCLK,
        REPVLD,
        REVMB: rx_pass_sel = pass_i;
        REPMB: rx_pass_sel = wdeg_l_pass_i & o_apply;
        default: rx_pass_sel = 1'b0;
        endcase
    end

    always_comb begin
        case (state)
        CAL: tx_pass_sel = serdes_tx_cal_done_i;
        REPCLK: tx_pass_sel = rsp_rclk_res_i & rclk_pass_i;
        REPVLD: tx_pass_sel = rsp_rvld_res_i & rvld_pass_i;
        REVMB: tx_pass_sel = rsp_rev_res_i & cmp_pass_i;
        REPMB: tx_pass_sel = (req_rep_degr_i & wdeg_r_pass_i) | o_rerun;
        default: tx_pass_sel = 1'b0;
        endcase
    end

    assign rx_pass = rx_pass_sel | rx_pass_d;
    assign tx_pass = tx_pass_sel | tx_pass_d;

    assign reg_width_deg_upd = s_repmb & o_apply_end;

    always_ff @(posedge clk_i) begin
        if (!rstn_i) begin
            rx_pass_d <= 1'b0;
            tx_pass_d <= 1'b0;
            reg_width_deg_upd_d <= 1'b0;
        end else begin
            rx_pass_d <= rx_pass & ~(state_change | o_clrerr);
            tx_pass_d <= tx_pass & ~(state_change | o_clrerr);
            reg_width_deg_upd_d <= reg_width_deg_upd;
        end
    end

    /* serdes outputs */
    assign serdes_rx_cal_req_o = s_cal & o_init;
    assign serdes_tx_cal_req_o = s_cal & o_init;

    /* general outputs */
    assign reg_lane_rev_o = ~abort;
    assign reg_lane_rev_upd_o = (s_revmb & o_result_end & ~tx_pass)
                                | (force_lane_rev_i & ~reg_lane_rev_i)
                                | abort;

    assign reg_width_deg_upd_o = reg_width_deg_upd_d;

    assign ping_stall_o = state_change;
    assign state_o = state;
    assign pt_start_o = s_repmb & (o_run | o_rerun);
    assign error_o = abort;
    assign done_o = abort | (s_repmb & state_done);
endmodule
