/*
 * assembly.s
 *
 */
 
 @ DO NOT EDIT
	.syntax unified
    .text
    .global ASM_Main
    .thumb_func

@ DO NOT EDIT
vectors:
	.word 0x20002000
	.word ASM_Main + 1

@ DO NOT EDIT label ASM_Main
ASM_Main:

	@ Some code is given below for you to start with
	LDR R0, RCC_BASE  		@ Enable clock for GPIOA and B by setting bit 17 and 18 in RCC_AHBENR
	LDR R1, [R0, #0x14]
	LDR R2, AHBENR_GPIOAB	@ AHBENR_GPIOAB is defined under LITERALS at the end of the code
	ORRS R1, R1, R2
	STR R1, [R0, #0x14]

	LDR R0, GPIOA_BASE		@ Enable pull-up resistors for pushbuttons
	MOVS R1, #0b01010101
	STR R1, [R0, #0x0C]
	LDR R1, GPIOB_BASE  	@ Set pins connected to LEDs to outputs
	LDR R2, MODER_OUTPUT
	STR R2, [R1, #0]
	MOVS R2, #0         	@ NOTE: R2 will be dedicated to holding the value on the LEDs

@ TODO: Add code, labels and logic for button checks and LED patterns


main_loop:
 	@ Check buttons on GPIOA
 	LDR R3, [R0, #0x10]

 	@ Check SW3
 	MOVS R4, #0x08
 	ANDS R4, R3, R4
 	BEQ sw3_pressed

 	@ Check SW2
 	MOVS R4, #0x04
 	ANDS R4, R3, R4
 	BEQ sw2_pressed

 	@ Determine increment amount
 	MOVS R5, #1
 	MOVS R4, #0x01
 	ANDS R4, R3, R4
 	BEQ sw0_pressed
 	B check_timing


sw0_pressed:
 	@ Increment by 2
 	MOVS R5, #2

check_timing:
	@ Determine delay timing
	MOVS R4, #0x02
	ANDS R4, R3, R4
	BEQ sw1_pressed

	@ Long delay; sw1 not pressed
	BL long_delay
	B do_increment

sw1_pressed:
 	@ Use short delay
	BL short_delay

@ Increment LED value
do_increment:

	ADDS R2, R2, R5  @ Add increment amount (R5)

	MOVS R4, #0xFF   @ Bitmask for 8 LEDs
	ANDS R2, R2, R4  @ Keep only 8 bits (wrap around)

	@ Shift
	@ADDS R2, R2, R2
	@CMP R2, #0
	@BEQ reset_led

	B write_leds

sw2_pressed:
	@ Set LEDs to 0xAA pattern
	MOVS R2, #0xAA
	B write_leds

sw3_pressed:
	@ Freeze
	B write_leds

reset_led:
	MOVS R2, #1


@ Delay subroutines
short_delay:
	PUSH {R6, LR}
	LDR R6, SHORT_DELAY_CNT
short_delay_loop:
	SUBS R6, R6, #1
	BNE short_delay_loop
	POP {R6, PC}

long_delay:
	PUSH {R6, LR}
	LDR R6, LONG_DELAY_CNT
long_delay_loop:
	SUBS R6, R6, #1
	BNE long_delay_loop
	POP {R6, PC}

write_leds:
	STR R2, [R1, #0x14]
	B main_loop

@ LITERALS; DO NOT EDIT
	.align
RCC_BASE: 			.word 0x40021000
AHBENR_GPIOAB: 		.word 0b1100000000000000000
GPIOA_BASE:  		.word 0x48000000
GPIOB_BASE:  		.word 0x48000400
MODER_OUTPUT: 		.word 0x5555

@ TODO: Add your own values for these delays
@ cnt = (delay)(frequency)/(cycles per iteration)
LONG_DELAY_CNT: 	.word 1400000
SHORT_DELAY_CNT: 	.word 600000
