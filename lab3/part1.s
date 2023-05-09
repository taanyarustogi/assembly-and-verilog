.global _start
.equ  KEY, 0xFF200050
.equ  HEX, 0xFF200020

_start:  ldr r0, = KEY		
	 ldr r4, = HEX		
	 mov  r2, #0

POLLING:    ldr r1, [r0]		
	    cmp r1, #0	
	    beq POLLING
	    cmp r1, #0x1 		
	    beq K0
	    cmp r1, #0x2			
	    beq K1
	    cmp r1, #0x4	
	    beq K2
	    cmp r1, #0x8			
	    beq K3
DONE:	    B	POLLING		
	
K0:   ldr r1, [r0]
      cmp r1, #0
      bgt K0
      mov r2, #0
      BL DISPLAY
      B POLLING

K1:   ldr r1, [r0]
      cmp r1, #0
      bgt K1
      cmp r2, #9
      beq STAY
      push {r2}
      ldr r2,[r4]
      cmp r2, #0
      beq STAY
      pop  {r2}
      add r2,#1

STAY: BL DISPLAY
      B  POLLING

K2:   ldr r1, [r0]
      cmp r1, #0
      bgt K2
      cmp r2, #0		
      beq STAY0
      sub r2, #1
	  
STAY0: BL DISPLAY
      B  POLLING

K3:   ldr r1, [r0]
      cmp r1, #0
      bgt K3
      mov r2, #0
      str r2,[r4]
      B POLLING   

DISPLAY:    MOV     R3, #BIT_CODES  
            push    {R2}
	    LDRB    R2, [R3,R2]    
            str     R2, [R4]
	    pop     {R2}
	    MOV     PC, LR     
			
BIT_CODES:  .byte   0b00111111, 0b00000110, 0b01011011, 0b01001111, 0b01100110
            .byte   0b01101101, 0b01111101, 0b00000111, 0b01111111, 0b01100111
            .skip   2      // pad with 2 bytes to maintain word alignment