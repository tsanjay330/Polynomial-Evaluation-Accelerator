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
              
  
  
