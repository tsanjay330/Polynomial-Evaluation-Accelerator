
/* This changes how the next_mode_in signal is utilized in the STATE_START 
	within FSM2
*/

case(state_module)
    STATE_START:
    begin
        case(next_mode_in)
        	SETUP_COMP: begin
            	next_state_module <= STATE_GET_COMMAND_START;
        	end
        
			COMP: begin 
				case(mode)//same mode signal that is passed to enable 
					STP: begin
            			next_state_module <= STATE_STP_START;
        			end

        			EVP: begin
            			next_state_module <= STATE_EVP_START;
        			end

        			EVB: begin
            			next_state_module <= STATE_EVB_START;
        			end
			
					//RST??
				endcase
			end

        	OUTPUT:	begin
            next_state_module <= STATE_OUTPUT;
        	end

        	default:
        	begin
            	next_state_module <= STATE_START;
        	end
       	endcase
    end
