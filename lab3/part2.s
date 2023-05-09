.global _start
.equ  KEY, 0xFF200050
.equ  HEX, 0xFF200020
_start:
		  LDR R1, =KEY		
		  LDR R4, =HEX		
		  MOV R0, #0
COUNT:    ADD R0, #1		
          BL DISPLAY
	  CMP R0, #100
          BEQ RESET				
	  B  DELAY

RESTART:  LDR R1, =KEY
	  LDR R2, [R1,#0xC]		
	  CMP R2, #0
	  BNE CHECK
	  B COUNT

RESET:	  MOV R0, #0
	  B COUNT

DELAY: LDR R7, = 500000
SUBTRACT: SUBS R7, R7, #1
          BNE SUBTRACT
          B  RESTART

CHECK: MOV R2, #15		
       STR R2, [R1,#0xC]
DELAYY:	   LDR R2, [R1,#0xC]
	   CMP R2, #0	
	   BEQ DELAY
	   MOV R2, #15	
	   STR R2, [R1,#0xC]
	   B RESTART

DISPLAY:   MOV  R3, #BIT_CODES
	   push {R2}
	   push {R0}
	   MOV R2, #0

DIVIDE:		CMP  R0, #10
		BLT  DONE
		SUB  R0, #10
	        ADD  R2, #1
		B DIVIDE

DONE:	LDRB    R0, [R3,R0]
	LDRB	R2, [R3,R2]
	LSL	R2, #8
	ORR	R0, R2
	STR 	R0, [R4]
	pop {R0}
	pop {R2}
        MOV     PC, LR              

BIT_CODES:  .byte   0b00111111, 0b00000110, 0b01011011, 0b01001111, 0b01100110
            .byte   0b01101101, 0b01111101, 0b00000111, 0b01111111, 0b01100111
            .skip   2    