	.syntax unified
	.cpu cortex-m4
	.thumb

	// input parameter of MAX7219Send
	addr	.req r0
	data	.req r1

	// variables for MAX7219Send
	cmd		.req r0
	BSRR	.req r1
	DIN		.req r2
	CS		.req r3
	CLK		.req r4
	cnt		.req r5

    // variable for Delay
    cntdown .req r0

    // variables for Display0toF
    addrarr	.req r2
    idarr	.req r3
    ptr		.req r4

.data
	student_id: .byte 8, 3, 0, 6, 1, 4, 0
	digit_addr:	.byte 1, 2, 3, 4, 5, 6, 7

.text
	.global main

	.equ RCC_AHB2ENR,	0x4002104C
	.equ GPIOB_MODER,	0x48000400
	.equ GPIOB_OSPEEDR,	0x48000408
	.equ GPIOB_BSRR,	0x48000418

	.equ DIGIT_0,		0x01
	.equ DECODE_MODE,	0x09
	.equ INTENSITY,		0x0A
	.equ SCAN_LIMIT,	0x0B
	.equ SHUTDOWN,		0x0C
	.equ DISPLAY_TEST,	0x0F

	.equ DIN_SET,		0x8
	.equ CS_SET,		0x10
	.equ CLK_SET,		0x20
	.equ DIN_RESET,		0x80000
	.equ CS_RESET,		0x100000
	.equ CLK_RESET,		0x200000

main:
    bl		GPIO_init
    bl		max7219_init

	ldr		addrarr, =digit_addr
	ldr		idarr, =student_id
    mov		ptr, #0

DisplayStudentID:
	ldrb	addr, [addrarr, ptr]
	ldrb	data, [idarr, ptr]
	bl		MAX7219Send

	add		ptr, #1
	cmp		ptr, #7
	bne		DisplayStudentID

Program_end:
	b		Program_end

MAX7219Send:	// send a message to max7219
	push	{r0, r1, r2, r3, r4, r5, r6}
	lsl		addr, #8
	add		cmd, addr, data

	ldr		BSRR,	=GPIOB_BSRR

	mov		cnt, #16
MAX7219SendLoop:
	mov  	r6, 0x1
	lsl		r6, cnt

	ldr		CLK, =CLK_RESET
	str 	CLK, [BSRR] // clk set 0

	ands	r6, cmd
	ite	eq
		ldreq	DIN, =DIN_RESET // din set 0
		ldrne	DIN, =DIN_SET // din set 1

	str 	DIN, [BSRR]

	ldr		CLK, =CLK_SET
	str		CLK, [BSRR] // clk set 1 to send 1 bit

	subs 	cnt, #1
	bge		MAX7219SendLoop
MAX7219SendDone:
	ldr		CS, =CS_RESET
	str  	CS, [BSRR] // cs set 0
	ldr		CS, =CS_SET
	str  	CS, [BSRR] // cs set 1

	pop		{r0, r1, r2, r3, r4, r5, r6}
	bx		lr

max7219_init:
	push	{lr}

	ldr		addr, =DECODE_MODE
	mov		data, #0xFF
	bl		MAX7219Send

	ldr		addr, =DISPLAY_TEST
	mov		data, #0
	bl		MAX7219Send

	ldr		addr, =SCAN_LIMIT
	mov		data, #6
	bl		MAX7219Send

	ldr		addr, =INTENSITY
	mov		data, #4
	bl		MAX7219Send

	ldr		addr, =SHUTDOWN
	mov		data, #1
	bl		MAX7219Send
DisplayClear:
	mov		addr, #8
	mov		data, #15
DisplayClearLoop:
	bl		MAX7219Send

	subs	addr, #1
	bne		DisplayClearLoop

	pop		{lr}
	bx		lr

GPIO_init:
	// enable AHB2 clock
	movs 	r0, #0x2
	ldr		r1, =RCC_AHB2ENR
	str		r0, [r1]

	// set PA0, PA1, PA2 as output mode
	mov		r0, #0x540
	ldr		r1, =GPIOB_MODER
	ldr		r2, [r1]
	and		r2, #0xFFFFF03F
	orr		r2, r2, r0
	str		r2, [r1]

	// set PA0, PA1, PA2 as high speed mode
	mov		r0, #0x540
	ldr		r1, =GPIOB_OSPEEDR
	ldr		r2, [r1]
	and		r2, #0xFFFFF03F
	orr		r2, r2, r0
	str		r2, [r1]

	bx		lr
