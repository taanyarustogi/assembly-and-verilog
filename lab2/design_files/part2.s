/* Program that counts consecutive 1's */                           
          .text                   // executable code follows
          .global _start                  
_start:                             
          MOV     R4, #TEST_NUM   // load the data word ...
          LDR     R1, [R4]        // into R1
		  MOV	  R5, #0
		  CMP	  R1, #0
		  BEQ     END
		  BL	  ONES
		  MOV     R5, R0
LOOOP:	  LDR     R1, [R4, #4]!
          CMP	  R1, #0
		  BEQ     END
		  BL	  ONES
		  CMP     R5, R0
		  BGE     LOOOP
		  MOV     R5, R0
		  B       LOOOP
		  
END:      B       END             

ONES:     MOV     R0, #0          // R0 will hold the result
LOOP:     CMP     R1, #0          // loop until the data contains no more 1's
          BEQ     DONE             
          LSR     R2, R1, #1      // perform SHIFT, followed by AND
          AND     R1, R1, R2      
          ADD     R0, #1          // count the string length so far
          B       LOOP
DONE:	  MOV	  PC, LR

TEST_NUM: .word   0x103fe00f   // 9
          .word   0xffffffff   // 20
          .word   0x10101010   // 1
          .word   0x1f1f1f1f   // 5
          .word   0x103ff00f   // a
          .word   0x100fe00f   // 7
          .word   0x103fe0ef   // 9
          .word   0x1f3fefff   // c
          .word   0x113fe00f   // 9
          .word   0x11111111   // 1
          .word   0x00000000  

          .end                             
