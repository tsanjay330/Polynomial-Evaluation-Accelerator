`timescale 1ns/1ps
module tb_PEA();

    parameter buffer_size = 1024, width = 16, buffer_size_out = 1;
	parameter GET_COMMAND = 3'b000, STP = 3'b001, EVP = 3'b010, EVB = 3'b011, OUTPUT = 3'b100, RST = 3'b101;

    /* Input vector size for the PEA. */
    parameter size = 5;//TODO: Different for each test?

    reg clk, rst;
    reg invoke;
    reg wr_en_input;
    reg [width - 1:0] data_in, command_in;
    reg [1 : 0]  next_mode_in;
    reg rd_en_result;
	reg rd_en_status;

    /* Input memories for PEA. */
    reg [width - 1 : 0] mem_data [0 : size - 1];
    reg [width - 1 : 0] mem_command [0 : size - 1];

    wire [1:0] next_mode_out;
    wire [width - 1 : 0] data_in_fifo_data, data_in_fifo_command, data_out_result, data_out_fifo1_result, data_out_status, data_out_fifo2_status;

    wire [log2(buffer_size) - 1:0] data_pop, command_pop;
    wire [log2(buffer_size_out) - 1 : 0] result_pop_out, status_pop_out;
    wire [log2(buffer_size) - 1 : 0] free_space_data, free_space_command;
    wire [log2(buffer_size_out) - 1 : 0] free_space_out_result, free_space_out_status;
    wire FC;
	
	/**** b and N wires ***/
	wire [4:0] b; 
	wire [3:0] N;

    integer i, j, k;

    /***************************************************************************
    Instantiate the input and output FIFOs for the actor under test.
    ***************************************************************************/

    fifo #(buffer_size, width)
            data_fifo
            (clk, rst, wr_en_input, /*rd_in_fifo_data*/, data_in,
            data_pop, free_space_data, data_in_fifo_data);

    fifo #(buffer_size, width) command_fifo
            (clk, rst, wr_en_input, rd_in_fifo_command, command_in,
            command_pop, free_space_command, data_in_fifo_command);

    fifo #(buffer_size_out, width) out_fifo1
            (clk, rst, wr_out_fifo1, rd_en_fifo1, data_out,
            pop_out_fifo1, free_space_out_fifo1, data_out_fifo1);

    /***************************************************************************
    Instantiate the enable and invoke modules for the actor under test.
    ***************************************************************************/

    stream_comp_invoke_top_module_1 #(.size(size), .width(width))
            invoke_module(clk, rst,
            data_in_fifo1_window, data_in_fifo2_L, data_in_fifo3_C,
            invoke, next_mode_in,
            rd_in_fifo1_window, rd_in_fifo2_L, rd_in_fifo3_C,
            next_mode_out, FC, wr_out_fifo1,
            data_out);

    stream_comp_enable #(.size(size), .buffer_size(buffer_size),
            .buffer_size_out(buffer_size_out)) enable_module(rst,
            pop_in_fifo1_window, pop_in_fifo2_L, pop_in_fifo3_C,
            free_space_out_fifo1, next_mode_in, enable);

    integer descr;

    /***************************************************************************
    Generate the clock waveform for the test.
    The clock period is 2 time units.
    ***************************************************************************/
    initial
    begin
        clk <= 0;
        for(j = 0; j < 100; j = j + 1)
        begin
            #1 clk <= 1;
            #1 clk <= 0;
        end
    end

    /***************************************************************************
    Try to carry out three actor firings (to cycle through the three
    CFDF modes of the actor under test).
    ***************************************************************************/
    initial
    begin
        /* Set up a file to store the test output */
        descr = $fopen("out.txt");

        /* Read text files and load the data into memory for input of inner 
        product actor
        */
        $readmemh("stream.txt", mem_one_window);
        $readmemh("length.txt", mem_two_L);
        $readmemh("cmd.txt", mem_three_C);

        #1;
        rst <= 0;
        wr_en_input <= 0;
        data_in_one_window <= 0;
        data_in_two_L <= 0;
        data_in_three_C <= 0;
        invoke <= 0;
        next_mode_in <= MODE_ONE;
        rd_en_fifo1 <= 0;
        #2 rst <= 1;
        #2;

        /* Write data into the input FIFOs. The FIFO requires a write enable
         * signal before the data is loaded, so "size" loop intereation are 
         * required here.
         */


        $fdisplay(descr, "Setting up input FIFOs");
        for (i = 0; i < size; i = i + 1)
        begin
               #2
               data_in_one_window <= mem_one_window[i];

               data_in_two_L <= mem_two_L[i];

               data_in_three_C <= mem_three_C[i];
               #2;
               wr_en_input <= 1;
               #2;
               wr_en_input  <= 0;
        end

        #2;     /* ensure that data is stored into memory before continuing */
        $fdisplay(descr, "pop_win = %d", pop_in_fifo1_window);
        $fdisplay(descr, "pop_L = %d", pop_in_fifo2_L);
        $fdisplay(descr, "pop_C = %d", pop_in_fifo3_C);
        next_mode_in <= MODE_ONE;
        #2;
        if (enable)
        begin
            $fdisplay(descr, "Executing firing for mode no. 1");
            invoke <= 1;
        end
        else
        begin
            /* end the simulation here if we don't have enough data to fire */
            $fdisplay (descr, "Not enough data to fire the actor under test");
            $finish;
        end
        #2 invoke <= 0;

        /* Wait for mode 1 to complete */
        wait (FC) #2 next_mode_in <= MODE_TWO;
        #2;
        if (enable)
        begin
            $fdisplay(descr, "Executing firing for mode no. 2");
            invoke <= 1;
        end
        else
        begin
            /* end the simulation here if we don't have enough data to fire */
            $fdisplay (descr, "Not enough data to fire the actor under test");
            $finish;
        end
        #2 invoke <= 0;

        /* Wait for mode 2 to complete */
        wait(FC) #2 next_mode_in <= MODE_THREE;
        #2;
        if (enable)
        begin
            $fdisplay(descr, "Executing firing for mode no. 3");
            invoke <= 1;
        end
        else
        begin
            /* end the simulation here if we don't have enough data to fire */
            $fdisplay (descr, "Not enough data to fire the actor under test");
            $finish;
        end
        #2 invoke <= 0;

        /* Wait for mode 3 to complete */
        wait(FC) #2;
        #2/* Read actor output value from result FIFO */
        rd_en_fifo1 <= 1;

        #2
        rd_en_fifo1 <= 0;

        #2;
        next_mode_in <= MODE_ONE;

        /* Set up recording of results */
        $fdisplay(descr, "time = %d, FIFO[0] = %d", $time, out_fifo1.FIFO_RAM[0]);
        $fdisplay(descr, "time = %d, Result = %d", $time, data_out_fifo1);
        $display("time = %d, Result = %d", $time, data_out_fifo1);
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



