/*******************************************************************************
@ddblock_begin copyright

Copyright (c) 1997-2022
Maryland DSPCAD Research Group, The University of Maryland at College Park 

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the above
copyright notice and the following two paragraphs appear in all copies
of this software.

IN NO EVENT SHALL THE UNIVERSITY OF MARYLAND BE LIABLE TO ANY PARTY
FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES
ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF
THE UNIVERSITY OF MARYLAND HAS BEEN ADVISED OF THE POSSIBILITY OF
SUCH DAMAGE.

THE UNIVERSITY OF MARYLAND SPECIFICALLY DISCLAIMS ANY WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE
PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF
MARYLAND HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
ENHANCEMENTS, OR MODIFICATIONS.

@ddblock_end copyright
*******************************************************************************/

/*******************************************************************************
This is an edited version of the single port RAM (random access memory) module.

--- INPUTS: ---

data:  data to be written into the RAM

wr_addr:  RAM address for writing data

rd_addr: RAM address for reading data

wr_en: enable signal for activating write operation (active high)

rd_en: enable signal for activating reading operation (active high)

clk: clock

--- OUTPUT: ---

q: data that is read out of the RAM

q_en: binary signal that goes high for a single clock cycle when data is successfully read from RAM

wr_suc: binary signal that goes high for a single clock cycle when data is successfully written to RAM

--- PARAMETERS: ---    

buffer_size: the number of tokens (integers) in each input vector. So, if size =
N, then this actor performs stream computation.  

word_size: the bit width for each of the input data  used in computation of the PEA actor.
*******************************************************************************/

`timescale 1ns/1ps
module N_ram
/*	Since this module is only for N - the word size is 4 bits and there are 8 coefficient vectors*/
        #(parameter word_size = 4, buffer_size = 8)(  
        input [word_size - 1 : 0] 	  data,
	input 				  rst,
        input [log2(buffer_size) - 1 : 0] wr_addr,
        input [log2(buffer_size) - 1 : 0] rd_addr,
        input 				  wr_en, re_en, clk,
        output reg [word_size - 1 : 0] 	  q,
	output reg 			  wr_suc,
	output reg 			  q_en); // This signal goes high when data is successfully read out - it is the responsibility of the module that drives the read_enable signal to make sure that re_en only stays high for a single clock cycle.

    /* Declare the RAM variable */
    reg [word_size - 1 : 0] ram[buffer_size - 1 : 0];
	
    /* Variable to hold the registered read address */
    reg [log2(buffer_size) - 1 : 0] addr_reg;

   integer 			    i; // Only used in loop for reset capabiltiy
	
    always @ (posedge clk)
      begin
	/* Reset functionality */
        if (rst) begin
	   for(i = 0; i < buffer_size; i = i+1) begin
	      ram[i] <= 4'b1111; // The "reset"/"error" degree value
	   end
	end
	/* Write */
        if (wr_en) begin
           ram[wr_addr] <= data;
	   wr_suc <= 1'b1;
	end
        else
	  wr_suc <= 1'b0;

        /* Read */
        if (re_en) begin
	   q <= ram[rd_addr];
	   q_en <= 1'b1;
	end
        else
	   q_en <= 1'b0;	  
    end
		
		/* Read - how it worked for the old RAM module */
    // assign q = (re_en) ? ram[rd_addr] : 0;
 
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