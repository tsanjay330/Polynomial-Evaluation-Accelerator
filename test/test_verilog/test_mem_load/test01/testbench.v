`timescale 1ns / 1ps
module testbench();
   parameter word_size = 16, buffer_size = 1024;
   parameter NUM_C_TOKENS = 3, NUM_D_TOKENS = 6;

   // External Inputs
   reg clk;
   reg rst;
   //reg rst_instr; // This signal is not actually used, but fills mem controller port list.
   
   // Inputs to FIFOs
   reg fifo_wr_en_c, fifo_wr_en_d;
   wire fifo_rd_en_c, fifo_rd_en_d; // Output from mem. controller to FIFOs
   reg [word_size - 1 : 0] fifo_in_c, fifo_in_d;

   // FIFO Outputs
   wire [log2(buffer_size) - 1 : 0] fifo_pop_c, fifo_pop_d;
   wire [log2(buffer_size) - 1 : 0] fifo_free_space_c, fifo_free_space_d;
   wire [word_size - 1 : 0] 	    fifo_out_c, fifo_out_d;

   // Additional RAM signals
   wire [word_size - 1 : 0] 	    ram_c_q, ram_d_q;
   reg [log2(buffer_size) - 1 : 0]  ram_c_rd_addr, ram_d_rd_addr;
   reg 				    ram_c_rd_en, ram_d_rd_en;
   

   // Inputs to memory controller
   reg 				    start_in_c, start_in_d;

   // Memory Controller Outputs
   wire 			    ram_c_wr_en, ram_d_wr_en;
   wire [log2(buffer_size) - 1 : 0] ram_c_wr_addr, ram_d_wr_addr;
   wire [word_size - 1 : 0]   	    mc_output_c, mc_output_d;
   
   // Additional Signals   
   integer i;
   reg [word_size - 1 : 0] input_mem_C [0 : NUM_C_TOKENS - 1];
   reg [word_size - 1 : 0] input_mem_D [0 : NUM_D_TOKENS - 1];
   
/********************************************************************
 Instantiate input FIFOs, RAM modules, and Memory Controllers/Loaders
 ********************************************************************/

  fifo #(buffer_size, word_size) fifo_in_command(clk, rst, fifo_wr_en_c, fifo_rd_en_c, fifo_in_c, fifo_pop_c, fifo_free_space_c, fifo_out_c);

  fifo #(buffer_size, word_size) fifo_in_data(clk, rst, fifo_wr_en_d, fifo_rd_en_d, fifo_in_d, fifo_pop_d, fifo_free_space_d, fifo_out_d);

   // *Note* - buffer size and word size parameters switch locations between fifos and everything else
   single_port_ram #(word_size, buffer_size) ram_command(.data(mc_output_c), .addr(ram_c_wr_addr), .rd_addr(ram_c_rd_addr), .wr_en(ram_c_wr_en), .re_en(ram_c_rd_en), .clk(clk), .q(ram_c_q));
   single_port_ram #(word_size, buffer_size) ram_data(.data(mc_output_d), .addr(ram_d_wr_addr), .rd_addr(ram_d_rd_addr), .wr_en(ram_d_wr_en), .re_en(ram_d_rd_en), .clk(clk), .q(ram_d_q));

   // Memory Controller
   mem_controller #(word_size, buffer_size) mem_controller_c(.clk(clk), .rst(rst), .FIFO_population(fifo_pop_c), .input_token(fifo_out_c), .start_in(start_in_c), .FIFO_rd_en(fifo_rd_en_c), .ram_wr_en(ram_c_wr_en), .ram_wr_addr(ram_c_wr_addr), .output_token(mc_output_c));
   mem_controller #(word_size, buffer_size) mem_controller_d(.clk(clk), .rst(rst), .FIFO_population(fifo_pop_d), .input_token(fifo_out_d), .start_in(start_in_d), .FIFO_rd_en(fifo_rd_en_d), .ram_wr_en(ram_d_wr_en), .ram_wr_addr(ram_d_wr_addr), .output_token(mc_output_d));
   
  integer descr, descr_c, descr_d; 
   
   
initial begin
   rst <= 1'b0;
   #4 rst <= 1'b1;
end
initial begin   
   // Clock alternates with a period of 2 time units
   for(i = 0; i < 200; i = i + 1) begin
      #1 clk <= 1;
      #1 clk <= 0;
   end

end

initial 
  begin
     //descr_c = $fopen("c_out.txt");
     //descr_d = $fopen("d_out.txt");
     descr = $fopen("out.txt");
     /*
     $readmemh("input_c.txt", input_mem_C);
     $readmemh("input_d.txt", input_mem_D);

     $fdisplay(descr, "Setting up input FIFOs");
     for(i = 0; i < NUM_C_TOKENS; i = i + 1)
       begin
	  #2;
	  fifo_in_c <= input_mem_C[i];
	  #2;
	  fifo_wr_en_c <= 1;
	  $fdisplay(descr, "input_c[%d] = %d", i, fifo_in_c);
	  #2;
	  fifo_wr_en_c <= 0;
       end
     */

     // Manually set each fifo command token and fifo write enable signal
     #2;
     fifo_in_c <= 10;
     #2;
     fifo_wr_en_c <= 1'b1;
     #2;
     fifo_wr_en_c <= 1'b0;
     
     #2;
     fifo_in_c <= 20;
     #2;
     fifo_wr_en_c <= 1'b1;
     #2;
     fifo_wr_en_c <= 1'b0;

     #2;
     fifo_in_c <= 30;
     #2;
     fifo_wr_en_c <= 1'b1;
     #2;
     fifo_wr_en_c <= 1'b0;
     
     for(i = 0; i < NUM_C_TOKENS; i = i + 1)
       begin
         $fdisplay(descr, "fifo_in_command.FIFO_RAM[%1d] = %d", i, fifo_in_command.FIFO_RAM[i]);
       end
     
     // Manually set each fifo data token and fifo write enable signal
     #2;
     fifo_in_d <= 100;
     #2;
     fifo_wr_en_d <= 1'b1;
     #2;
     fifo_wr_en_d <= 1'b0;

     #2;
     fifo_in_d <= 200;
     #2;
     fifo_wr_en_d <= 1'b1;
     #2;
     fifo_wr_en_d <= 1'b0;

     #2;
     fifo_in_d <= 300;
     #2;
     fifo_wr_en_d <= 1'b1;
     #2;
     fifo_wr_en_d <= 1'b0;
     
     #2;
     fifo_in_d <= 400;
     #2;
     fifo_wr_en_d <= 1'b1;
     #2;
     fifo_wr_en_d <= 1'b0;

     #2;
     fifo_in_d <= 500;
     #2;
     fifo_wr_en_d <= 1'b1;
     #2;
     fifo_wr_en_d <= 1'b0;

     #2;
     fifo_in_d <= 600;
     #2;
     fifo_wr_en_d <= 1'b1;
     #2;
     fifo_wr_en_d <= 1'b0;
     
     for(i = 0; i < NUM_D_TOKENS; i = i + 1)
       begin
         $fdisplay(descr, "fifo_in_data.FIFO_RAM[%1d] = %d", i, fifo_in_data.FIFO_RAM[i]);
       end

     // Send in start signals to command and data memory controllers
     #2;
     start_in_c <= 1'b1;
     start_in_d <= 1'b1;

     // Wait for mem_controller to send FIFO tokens to RAM
     #100;
     start_in_c <= 1'b0;
     start_in_d <= 1'b0;

     #100;
     // Display contents of command and data ram modules
     for(i = 0; i < NUM_C_TOKENS; i = i + 1)
       begin
         $fdisplay(descr, "ram_command.ram[%1d] = %d", i, ram_command.ram[i]);
       end
     for(i = 0; i < NUM_D_TOKENS; i = i + 1)
       begin
         $fdisplay(descr, "ram_data.ram[%1d] = %d", i, ram_data.ram[i]);
       end
      
    // $fdisplay(descr, "ram_command.ram[%1d] = %d", 0, ram_command.ram[0]);
     
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
