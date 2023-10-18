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
