`timescale 1ns/1ps
module tb_PEA();
	
    parameter d_size = 4; // This is related to the loop needs to be specified for EACH command you are going to call.
	parameter c_size = 2; //Number of commands to pass in

	parameter SETUP_INSTR = 2'b00, INSTR = 2'b01, OUTPUT = 2'b10;
    parameter buffer_size = 1024, width = 16, buffer_size_out = 32;

    reg clk, rst;
    reg invoke;
    reg wr_en_data;
	reg wr_en_command;
    reg [width - 1:0] data_in, command_in;
    reg [1 : 0]  next_instr;
    reg rd_en_result;
	reg rd_en_status;

    /* Input memories for PEA. */
    reg [width - 1 : 0] mem_data [0 : d_size - 1];
    reg [width - 1 : 0] mem_command [0 : c_size - 1];

    wire [1:0] next_mode_out;
    wire [width - 1 : 0] data_in_fifo_data, data_in_fifo_command;
    wire [2*width - 1 : 0] data_out_result, data_out_fifo1_result, data_out_status, data_out_fifo2_status;

    wire [log2(buffer_size) - 1:0] data_pop, command_pop;
    wire [log2(buffer_size_out) - 1 : 0] result_pop_out, status_pop_out;
    wire [log2(buffer_size) - 1 : 0] free_space_data, free_space_command;
    wire [log2(buffer_size_out) - 1 : 0] free_space_out_result, free_space_out_status;
    wire FC;
	
	/**** b and N wires ***/
	wire [4:0] arg2; 

	//These signals come from the FSM2/3 level
	wire wr_out, rd_in_command, rd_in_data;
	wire [7:0] instr;
	wire start_in;
    integer i, j, k;

	wire [1:0] state_GC;
	wire [15:0] ram_out_c; 
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

	PEA_top_module_1 invoke_module(clk,rst,data_in_fifo_command, 
			data_in_fifo_data, invoke,next_instr, data_pop, command_pop, rd_in_command, rd_in_data, FC, wr_out, data_out_result, data_out_status, instr, arg2);
		
	PEA_enable enable_module(command_pop, data_pop, free_space_out_result,
			free_space_out_status, next_instr, instr, arg2, enable);

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
    $monitor("STATE:%1d, N:%1d, X:%1d, C_i:%1d, sum:%1d, xpow:%d",
		invoke_module.FSM2.evp_command.state,
        invoke_module.FSM2.evp_command.N,
		invoke_module.FSM2.evp_command.x,
        invoke_module.FSM2.evp_command.c_i,
        invoke_module.FSM2.evp_command.sum,
        0 // x_power is not a signal within the EVP module
		 //invoke_module.FSM2.evp_command.x_power        
		);    

/*
	$monitor("FSM1:%1d,FSM2:%1d, FSM3_DMEM:%1d, FSM3_STP:%1d,FSM3_EVP:%1d, x:%1d, c:%1d",
        invoke_module.state_module,
        invoke_module.FSM2.state_module,
        invoke_module.FSM2.DATA_MEM_CONTROLLER.state,
        invoke_module.FSM2.stp_command.state,
        invoke_module.FSM2.evp_command.state,
        invoke_module.FSM2.evp_command.N,
        invoke_module.FSM2.evp_command.result);	
*/


		/* Set up a file to store the test output */
        descr = $fopen("out.txt");

        /* Read text files and load the data into memory for input of PEA actor
        */
        $readmemh("data.txt", mem_data);
        $readmemh("command.txt", mem_command);

        #1;
        rst <= 0;
        wr_en_data <= 0;
		wr_en_command <= 0;
        data_in <= 0;
        command_in <= 0;
        invoke <= 0;
        next_instr <= SETUP_INSTR;
        rd_en_result <= 0;
		rd_en_status <=0;
        #2 rst <= 1;
        #2;

        /* Write data into the input FIFOs. The FIFO requires a write enable
         * signal before the data is loaded, so "size" loop intereation are 
         * required here.
         */
        
		for(i = 0; i < c_size ; i = i + 1)
		begin 
			#2
        	command_in <= mem_command[i];
			#2
			$fdisplay(descr, "input_c[%d] = %b", i, command_in);
			wr_en_command <= 1;
			#2
			wr_en_command <= 0;
		end
		#12
        if (1)//WILL BE ENABLE AFTER EDIT
        begin
            $fdisplay(descr, "Enable Passed!");
            invoke <= 1;
        end
        else
        begin
            /* End the simulation here if we don't have enough data to fire */
            $fdisplay (descr, "Enable Failed.");
            $finish;
        end
		#2
		invoke <= 0;
		
		$fdisplay(descr, "Waiting for GC to finish...");
		wait(FC);
       	#2
		$fdisplay(descr, "GC finished.");


		next_instr = INSTR;
		for(i = 0; i < d_size ; i = i + 1)
        begin
            #2
            command_in <= mem_command[i];
            data_in <= mem_data[i];
            #2
            $fdisplay(descr, "input_d[%d] = %b", i, data_in);
            wr_en_data <= 1;
            #2
            wr_en_data <= 0;
        end
		//Ensure data is read in
		#12
		next_instr = INSTR;
		#2
		if (1)//ENABLE NEEDS TO BE EDITED BEFORE ADDED BACK IN IF STATEMENT
        begin
            $fdisplay(descr, "Enable Passed!");
            invoke <= 1;
        end
        else
        begin
            /* End the simulation here if we don't have enough data to fire */
            $fdisplay (descr, "Enable Failed.");
            $finish;
        end
        #2
        invoke <= 0;
		 $fdisplay(descr, "Waiting for STP to finish...");
		
        wait(FC);
		$fdisplay(descr, "STP finished.");
        #2


// NOW ONTO SETUP EVP
		#2
        next_instr = SETUP_INSTR;
        #2
        if (1)//ENABLE NEEDS TO BE EDITED BEFORE ADDED BACK IN IF STATEMENT
        begin
            $fdisplay(descr, "Enable Passed!");
            invoke <= 1;
        end
        else
        begin
            /* End the simulation here if we don't have enough data to fire */
            $fdisplay (descr, "Enable Failed.");
            $finish;
        end
        #2
        invoke <= 0;
         $fdisplay(descr, "Waiting for GC to finish...");
        wait(FC);
		$fdisplay(descr, "GC finished.");
        #2


//NOW DO EVP
        next_instr = INSTR;
        #2
        if (1)//ENABLE NEEDS TO BE EDITED BEFORE ADDED BACK IN IF STATEMENT
        begin
            $fdisplay(descr, "Enable Passed!");
            invoke <= 1;
        end
        else
        begin
            /* End the simulation here if we don't have enough data to fire */
            $fdisplay (descr, "Enable Failed.");
            $finish;
        end
        #2
        invoke <= 0;
         $fdisplay(descr, "Waiting for EVP to finish...");
        wait(FC);
		$fdisplay(descr, "EVP finished.");
  
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



