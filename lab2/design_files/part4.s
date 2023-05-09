.text
.global _start                  
_start:         MOV     SP, #0x20000
				MOV     R4, #TEST_NUM   	// load the data word ...
				LDR		R1, [R4]
          		MOV     R5, #0
				MOV     R6, #0
				MOV     R7, #0
				CMP     R1, #0
		  		BEQ     DISPLAY
		  		BL      ONES
				BL		ZEROS
				BL		ALTERNATE
				MOV		R5, R8
				MOV		R6, R9
				MOV 	R7, R10
				
LOOP:    		LDR     R1, [R4, #4]!
		  		CMP     R1, #0
		  		BEQ     DISPLAY
		  		BL      ONES
		  		CMP     R5, R8          
		  		MOVLT   R5, R8
		  		BL      ZEROS
		  		CMP     R6, R9         
		  		MOVLT   R6, R9
				BL      ALTERNATE
				CMP     R7, R10
				MOVLT   R7, R10
				B       LOOP
           
END:      		B       END             

ONES:	  		MOV     R8, #0
				MOV     R2, R1
LOOPO:    		CMP     R2, #0          	// loop until the data contains no more 1's
			  	BEQ     DONEO             
			  	LSR     R0, R2, #1      	// perform SHIFT, followed by AND
			  	AND     R2, R2, R0      
			  	ADD     R8, #1          	// count the string length so far
			  	B       LOOPO
DONEO:  	 	MOV     PC, LR          	

ZEROS:	  		MOV     R9, #0	
				MOV 	R3, R1
LOOPZ:    		CMP     R3, #0xffffffff	
                BEQ		DONEZ           
				LSL     R0, R3, #1
				ADD     R0, #1 				
				ORR     R3, R3, R0	
				ADD     R9, #1
				B       LOOPZ
DONEZ:          MOV     PC, LR

ALTERNATE:      MOV     R0, #AS
				LDR     R0, [R0]
				MOV     R11, R1
				EOR     R1, R1, R0
				PUSH    {LR}
				BL      ONES
				POP     {LR}
				PUSH    {LR}
				BL      ZEROS
				POP     {LR}
				MOV     R10, R9
				CMP     R8, R10
				MOVGT   R10, R8
				CMP     R10, #2
				MOVLT   R10, #0
				MOV     R1, R11
				MOV     PC, LR     
		  
/* Subroutine to convert the digits from 0 to 9 to be shown on a HEX display.
 *    Parameters: R0 = the decimal value of the digit to be displayed
 *    Returns: R0 = bit patterm to be written to the HEX display
 */

SEG7_CODE:  MOV     R1, #BIT_CODES  
            ADD     R1, R0         // index into the BIT_CODES "array"
            LDRB    R0, [R1]       // load the bit pattern (to be returned)
            MOV     PC, LR              

BIT_CODES:  .byte   0b00111111, 0b00000110, 0b01011011, 0b01001111, 0b01100110
            .byte   0b01101101, 0b01111101, 0b00000111, 0b01111111, 0b01100111
            .skip   2      // pad with 2 bytes to maintain word alignment

/* Display R5 on HEX1-0, R6 on HEX3-2 and R7 on HEX5-4 */
DISPLAY:    LDR     R8, =0xFF200020 // base address of HEX3-HEX0
            MOV     R0, R5          // display R5 on HEX1-0
            BL      DIVIDE          // ones digit will be in R0; tens
                                    // digit in R1
            MOV     R9, R1          // save the tens digit
            BL      SEG7_CODE       
            MOV     R4, R0          // save bit code
            MOV     R0, R9          // retrieve the tens digit, get bit
                                    // code
            BL      SEG7_CODE       
            LSL     R0, #8
            ORR     R4, R0
            
            //code for R6 (not shown)
			MOV     R0, R6          // display R6 on HEX3-2
            BL      DIVIDE          // ones digit will be in R0; tens
                                    // digit in R1
            MOV     R9, R1          // save the tens digit
            BL      SEG7_CODE       
            LSL     R0, #16
            ORR     R4, R0            
			MOV     R0, R9          // retrieve the tens digit, get bit
                                    // code
            BL      SEG7_CODE
			LSL     R0, #24
            ORR     R4, R0
            
            STR     R4, [R8]        // display the numbers from R6 and R5
            LDR     R8, =0xFF200030 // base address of HEX5-HEX4
            
            //code for R7 (not shown)
			MOV     R0, R7          // display R7 on HEX5-4
            BL      DIVIDE          // ones digit will be in R0; tens
                                    // digit in R1
            MOV     R9, R1          // save the tens digit
            BL      SEG7_CODE       
            MOV     R4, R0          // save bit code
            MOV     R0, R9          // retrieve the tens digit, get bit
                                    // code
            BL      SEG7_CODE       
            LSL     R0, #8
            ORR     R4, R0
            
            STR     R4, [R8]        // display the number from R7
			B		END
			
DIVIDE:     MOV    R2, #0
CONT:       CMP    R0, #10
            BLT    DIV_END
            SUB    R0, #10
            ADD    R2, #1
            B      CONT
DIV_END:    MOV    R1, R2     // quotient in R1 (remainder in R0)
            MOV    PC, LR

AS: .word 0x55555555

TEST_NUM: .word   0x103fe00f   
          .word   0x10101010   
          .word   0xf10e0B51   
          .word   0xf00f7f3f   
          .word   0x01020304   
          .word   0x5f5e5553   
          .word   0x55555555   
          .word   0x12345432   
          .word   0x12312312   
          .word   0x00f00f00   
          .word   0x00000000

          .end 
