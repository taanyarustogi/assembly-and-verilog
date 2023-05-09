               .equ      EDGE_TRIGGERED,    0x1
               .equ      LEVEL_SENSITIVE,   0x0
               .equ      CPU0,              0x01    // bit-mask; bit 0 represents cpu0
               .equ      ENABLE,            0x1

               .equ      KEY0,              0b0001
               .equ      KEY1,              0b0010
               .equ      KEY2,              0b0100
               .equ      KEY3,              0b1000

               .equ      IRQ_MODE,          0b10010
               .equ      SVC_MODE,          0b10011

               .equ      INT_ENABLE,        0b01000000
               .equ      INT_DISABLE,       0b11000000

/*********************************************************************************
 * Initialize the exception vector table
 ********************************************************************************/
                .section .vectors, "ax"

                B        _start             // reset vector
                .word    0                  // undefined instruction vector
                .word    0                  // software interrrupt vector
                .word    0                  // aborted prefetch vector
                .word    0                  // aborted data vector
                .word    0                  // unused vector
                B        IRQ_HANDLER        // IRQ interrupt vector
                .word    0                  // FIQ interrupt vector

/* ********************************************************************************
 * This program demonstrates use of interrupts with assembly code. The program 
 * responds to interrupts from a timer and the pushbutton KEYs in the FPGA.
 *
 * The interrupt service routine for the timer increments a counter that is shown
 * on the red lights LEDR by the main program. The counter can be stopped/run by 
 * pressing any of the KEYs.
 ********************************************************************************/
                .text
                .global  _start
_start:                                         
/* Set up stack pointers for IRQ and SVC processor modes */
                  mov      r11, #0b11010010
				  msr      cpsr_c, r11
				  ldr      sp,=0x20000
				  mov      r11, #0b11010011
				  msr      cpsr_c, r11
				  ldr      sp,=0x40000
				  

                  BL       CONFIG_GIC         // configure the ARM generic
                                              // interrupt controller
                  BL       CONFIG_PRIV_TIMER  // configure A9 Private Timer
                  BL       CONFIG_KEYS        // configure the pushbutton
                                              // KEYs port

/* Enable IRQ interrupts in the ARM processor */
return:          mov      r11, #0b01010011
				  msr      cpsr_c, r11
                  LDR      R5, =0xFF200000    // LEDR base address
LOOP:                                          
                  LDR      R3, COUNT          // global variable
                  STR      R3, [R5]           // write to the LEDR lights
                  B        LOOP                
          

/* Global variables */
                .global  COUNT
COUNT:          .word    0x0                  // used by timer
                .global  RUN
RUN:            .word    0x1                  // initial value to increment COUNT
                .global  speed
speed:         .word    0b10111110101111000001111111
/* Configure the A9 Private Timer to create interrupts at 0.25 second intervals */
CONFIG_PRIV_TIMER:                             
                ldr      r0, =0xfffec600
				ldr      r3, speed
				str      r3, [r0]
				mov      r3, #7
				str      r3, [r0, #8]
				mov      r3, #1
				str      r3, [r0, #12]
                MOV      PC, LR
                   
/* Configure the pushbutton KEYS to generate interrupts */
CONFIG_KEYS:                                    
                ldr      r0, =0xff200050
				mov      r1, #0xf
				str      r1, [r0, #0x8]
                MOV      PC, LR

/*--- IRQ ---------------------------------------------------------------------*/
IRQ_HANDLER:
                push     {r0-r7, lr}
				ldr      r4, =0xfffec100
				ldr      r5, [r4, #0x0c]
				cmp      r5, #73
				beq      KEY_ISR
				cmp      r5, #29
				beq      PRIV_TIMER_ISR

EXIT_IRQ:	  str      r5, [r4, #0x10]
               pop      {r0-r7, lr}
			   SUBS     PC, LR, #4

/****************************************************************************************
 * Pushbutton - Interrupt Service Routine                                
 *                                                                          
 * This routine toggles the RUN global variable.
 ***************************************************************************************/
                .global  KEY_ISR
KEY_ISR:        
                ldr		r0, =0xff200050
				ldr     r1, [r0, #0xc]
				mov		r2, #0xf
				str		r2, [r0,#0xc] //reset edge capture first
				
				ldr		r0, =0xff200000
				cmp     r1, #0x1
				beq     checkK0
				cmp     r1, #0x2
				beq     checkK1
				cmp     r1, #0x4
				beq     checkK2
				cmp     r1, #0x8
				beq     EXIT_IRQ
				
checkK0:	   ldr     r1, RUN
			    eor     r1, #1
				str     r1, RUN
				ldr     r2, COUNT
				add     r2, r1
				str     r2, COUNT
				str     r2, [r0]
				
				b       EXIT_IRQ

checkK1:      ldr      r1, speed
			   lsr      r1, #1
			   str      r1, speed
			   ldr      r3, =0xfffec600
			   str      r1, [r3]
			   
			   b       EXIT_IRQ

checkK2:      ldr      r1, speed
			   lsl      r1, #1
			   str      r1, speed
			   ldr      r3, =0xfffec600
			   str      r1, [r3]
			   
			   b       EXIT_IRQ



/*				ldr		r1, RUN
				eor		r1,#1
				str 	r2, RUN //revert RUN each time
				ldr		r2, COUNT
				add		r2, r1
				str		r2, COUNT //if stopped, remain the same, or display COUNT+1
				str     r2, [r0] //display LEDs
				
                B		LEAVE_IRQ

                //MOV      PC, LR*/

/******************************************************************************
 * A9 Private Timer interrupt service routine
 *                                                                          
 * This code toggles performs the operation COUNT = COUNT + RUN
 *****************************************************************************/
                .global    TIMER_ISR
PRIV_TIMER_ISR:
                ldr      r0, =0xfffec600
				mov      r3, #1
				str      r3,[r0, #12]
				ldr      r2, COUNT
				ldr      r0, RUN
				add      r2, r0
				ldr      r3, =0b1111111111
				cmp      r2, r3
				bgt      DELETE 
				str      r2, COUNT
				b        END_TIMER

DELETE:        mov      r1, #0
			   str      r1, COUNT
				
END_TIMER:    b         EXIT_IRQ  
/* 
 * Configure the Generic Interrupt Controller (GIC)
*/
                .global  CONFIG_GIC
CONFIG_GIC:
                PUSH     {LR}
                MOV      R0, #29
                MOV      R1, #CPU0
                BL       CONFIG_INTERRUPT
                
                /* Enable the KEYs interrupts */
                MOV      R0, #73
                MOV      R1, #CPU0
                /* CONFIG_INTERRUPT (int_ID (R0), CPU_target (R1)); */
                BL       CONFIG_INTERRUPT

                /* configure the GIC CPU interface */
                LDR      R0, =0xFFFEC100        // base address of CPU interface
                /* Set Interrupt Priority Mask Register (ICCPMR) */
                LDR      R1, =0xFFFF            // enable interrupts of all priorities levels
                STR      R1, [R0, #0x04]
                /* Set the enable bit in the CPU Interface Control Register (ICCICR). This bit
                 * allows interrupts to be forwarded to the CPU(s) */
                MOV      R1, #1
                STR      R1, [R0]
    
                /* Set the enable bit in the Distributor Control Register (ICDDCR). This bit
                 * allows the distributor to forward interrupts to the CPU interface(s) */
                LDR      R0, =0xFFFED000
                STR      R1, [R0]    
    
                POP      {PC}
/* 
 * Configure registers in the GIC for an individual interrupt ID
 * We configure only the Interrupt Set Enable Registers (ICDISERn) and Interrupt 
 * Processor Target Registers (ICDIPTRn). The default (reset) values are used for 
 * other registers in the GIC
 * Arguments: R0 = interrupt ID, N
 *            R1 = CPU target
*/
CONFIG_INTERRUPT:
                PUSH     {R4-R5, LR}
    
                /* Configure Interrupt Set-Enable Registers (ICDISERn). 
                 * reg_offset = (integer_div(N / 32) * 4
                 * value = 1 << (N mod 32) */
                LSR      R4, R0, #3               // calculate reg_offset
                BIC      R4, R4, #3               // R4 = reg_offset
                LDR      R2, =0xFFFED100
                ADD      R4, R2, R4               // R4 = address of ICDISER
    
                AND      R2, R0, #0x1F            // N mod 32
                MOV      R5, #1                   // enable
                LSL      R2, R5, R2               // R2 = value

                /* now that we have the register address (R4) and value (R2), we need to set the
                 * correct bit in the GIC register */
                LDR      R3, [R4]                 // read current register value
                ORR      R3, R3, R2               // set the enable bit
                STR      R3, [R4]                 // store the new register value

                /* Configure Interrupt Processor Targets Register (ICDIPTRn)
                  * reg_offset = integer_div(N / 4) * 4
                  * index = N mod 4 */
                BIC      R4, R0, #3               // R4 = reg_offset
                LDR      R2, =0xFFFED800
                ADD      R4, R2, R4               // R4 = word address of ICDIPTR
                AND      R2, R0, #0x3             // N mod 4
                ADD      R4, R2, R4               // R4 = byte address in ICDIPTR

                /* now that we have the register address (R4) and value (R2), write to (only)
                 * the appropriate byte */
                STRB     R1, [R4]
    
                POP      {R4-R5, PC}
                .end   