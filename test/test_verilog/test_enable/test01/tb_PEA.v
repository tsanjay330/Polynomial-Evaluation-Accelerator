`timescale 1ns/1ps
module tb_PEA();
	
    parameter SIZE; // This is related to the loop needs to be specified for EACH command you are going to call.
	parameter SETUP_INSTR = 2'b00, INSTR = 2'b01, OUTPUT = 2'b10;
    parameter buffer_size = 1024, width = 16, buffer_size_out = 1;
	parameter GET_COMMAND = 3'b000, STP = 3'b001, EVP = 3'b010, EVB = 3'b011, OUTPUT = 3'b100, RST = 3'b101;

    reg clk, rst;
    reg invoke;
    reg wr_en_data;
	reg wr_en_command;
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

	//These signals come from the FSM2/3 level
	wire wr_out, rd_in_command, rd_in_data;
	wire [2:0] mode;

    integer i, j, k;

    /***************************************************************************
    Instantiate the input and output FIFOs for the actor under test.
    ***************************************************************************/

    fifo #(buffer_size, width)
            data_fifo
            (clk, rst, wr_en_data, rd_in_data, data_in,
            data_pop, free_space_data, data_in_fifo_data);

    fifo #(buffer_size, width) command_fifo
            (clk, rst, wr_en_command, rd_in_command, command_in,
            command_pop, free_space_command, data_in_fifo_command);

	/* OUTPUT FIFOS*/
    fifo #(buffer_size_out, 32) out_fifo_result
            (clk, rst, wr_out, rd_en_result, data_out_result,
            result_pop_out, free_space_out_result, data_out_fifo1_result);

	fifo #(buffer_size_out, 32) out_fifo_status
            (clk, rst, wr_out, rd_en_status, data_out_status,
            status_pop_out, free_space_out_status, data_out_fifo2_status);


    /***************************************************************************
    Instantiate the enable and invoke modules for the actor under test.
    ***************************************************************************/

	PEA_top_module_1 invoke_module(clk,rst,command_in, data_in, invoke, 
			next_mode_in, rd_in_command, rd_in_data, FC, wr_out, 
			data_out_result,data_out_status, mode, b, N);
		
	PEA_enable enable_module(command_pop, data_pop, free_space_out_result,
			free_space_out_status, next_mode_in, mode, b, N, enable);

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

        /* Read text files and load the data into memory for input of PEA actor
        */
        $readmemh("data.txt", mem_data);
        $readmemh("command.txt", mem_command);

        #1;
        rst <= 0;
        wr_en_input <= 0;
        data_in <= 0;
        command_in <= 0;
        invoke <= 0;
        next_mode_in <= SETUP_INSTR;
        rd_en_result <= 0;
		rd_en_status <=0;
        #2 rst <= 1;
        #2;

        /* Write data into the input FIFOs. The FIFO requires a write enable
         * signal before the data is loaded, so "size" loop intereation are 
         * required here.
         */
         
        command_in <= mem_command[i];
		#2
		wr_en_command <= 1;
		#2
		wr_en_input <= 0;

        $fdisplay(descr, "Setting up input FIFOs");
        for (i = 0; i < SIZE ; i = i + 1)
        begin
               #2
               data_in <= mem_data[i];
               #2;
               wr_en_data <= 1;
               #2;
               wr_en_data  <= 0;
        end

        #2;     /* ensure that data is stored into memory before continuing */
        next_mode_in <= SETUP_INSTR;
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
        wait (FC) #2 next_mode_in <= INSTR;
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
        wait(FC) #2 next_mode_in <= OUTPUT;
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
        rd_en_result <= 1;
		rd_en_status <= 1;
        #2
        rd_en_result <= 0;
		rd_en_status <= 0;
        #2;
        next_mode_in <= SETUP_INSTR;

        /* Set up recording of results */
        //TODO: Not sure about this fdisplay with the out_fifo1.FIFORAM[]
		$fdisplay(descr, "time = %d, FIFO[0] = %d", $time, out_fifo1.FIFO_RAM[0]);

        $fdisplay(descr, "time = %d, Result = %d, Status = %d", $time, 
					data_out_fifo1_result, data_out_fifo2_status);
        $display("time = %d, Result = %d, Status = %d", $time, 
			data_out_fifo1_result, data_out_fifo2_status);
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



