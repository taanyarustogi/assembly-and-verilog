.text
.global _start                  
_start:         MOV     SP, #0x20000
				MOV     R4, #TEST_NUM   	// load the data word ...
				LDR		R1, [R4]
          		MOV     R5, #0
				MOV     R6, #0
				MOV     R7, #0
				CMP     R1, #0
		  		BEQ     END
		  		BL      ONES
				BL		ZEROS
				BL		ALTERNATE
				MOV		R5, R8
				MOV		R6, R9
				MOV 	R7, R10
				
LOOP:    		LDR     R1, [R4, #4]!
		  		CMP     R1, #0
		  		BEQ     END
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