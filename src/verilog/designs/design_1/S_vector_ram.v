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
This is the RAM module designed exclusively for the S coefficient vector array.

--- INPUTS: ---

data:  data to be written into the RAM

rd_vector_index:  The index/address of the vector within S that is being read

rd_coef_index: The index/address of the coefficient within Si that is being read
 
wr_vector_index:  The index/address for which vector within S is being written to

wr_coef_index: The index/address for which coefficient within Si is being written to

wr_en: enable signal for activating write operation (active high)

rd_en: enable signal for activating reading operation (active high)

clk: clock

--- OUTPUT: ---

q: data that is read out of the RAM

q_en: Binary signal that is meant to go high for a single clock cycle when data is successfully read out of RAM.
 
wr_suc: binary signal that is meant to go high for a signel clock cycle when data is successfully written into RAM.

--- PARAMETERS: ---    
word_size: the bit width for each of the input data used in computation of the PEA actor.
 
num_vectors: The number of coefficient vectors (in this project, 8)
 
max_degree: The maximum degree of each polynomial (in this project, 10)
*******************************************************************************/

`timescale 1ns/1ps
module S_vector_ram
/*Here word_size and buffer_size is treated as width and size parameters from lab 07 respectively*/
        #(parameter word_size = 16, num_vectors = 8, max_degree = 10)(  
        input [word_size - 1 : 0] 	  data,
        input [log2(num_vectors) - 1 : 0] rd_vector_addr,
        input [log2(max_degree) : 0] 	  rd_coef_addr,
	input [log2(num_vectors) - 1 : 0] wr_vector_addr,
        input [log2(max_degree) : 0] 	  wr_coef_addr,
        input 				  wr_en, re_en, clk,
        output [word_size - 1 : 0] 	  q,
	output wr_suc,
	output 				  q_en); // This signal goes high when data is successfully read out - it is the responsibility of the module that drives the read_enable signal to make sure that re_en only stays high for a single clock cycle.

    /* Declare the RAM variable */
    reg [word_size - 1 : 0] ram[num_vectors - 1 : 0][max_degree : 0];
	
    /* Variable to hold the registered read address */
    reg [log2(buffer_size) - 1 : 0] addr_reg;
	
    always @ (posedge clk)
    begin
        /* Write */
        if (wr_en) begin
           ram[wr_vector_addr][wr_coef_addr] <= data;
	   wr_suc <= 1'b1;
	end
        else
	  wr_suc <= 1'b0;
       
        /* Read */
        if (re_en) begin
	   q <= ram[rd_vector_addr][rd_coef_addr];
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
