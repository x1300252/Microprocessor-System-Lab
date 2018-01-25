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
    len			.req r0
    digit		.req r1
    lenarr		.req r2
    digitarr	.req r3
    lenptr		.req r4
    digitptr	.req r5
    C_IDR		.req r6

.data
	len: 	.byte 1,1,1,1,1,1,1,  2,  2,  2,  2,  2,    3,    3,    3,    3,    3,      4,      4,      4,      4,        5,        5,        5,        5,        5,          6,          6,          6,          6,          6,            7,            7,            7,            7,            7,              8,              8,              8,              8
	digit:	.byte 0,1,1,2,3,5,8,1,3,2,1,3,4,5,5,8,9,1,4,4,2,3,3,3,7,7,6,1,0,9,8,7,1,5,9,7,2,5,8,4,4,1,8,1,6,7,6,5,1,0,9,4,6,1,7,7,1,1,2,8,6,5,7,4,6,3,6,8,7,5,0,2,5,1,2,1,3,9,3,1,9,6,4,1,8,3,1,7,8,1,1,5,1,4,2,2,9,8,3,2,0,4,0,1,3,4,6,2,6,9,2,1,7,8,3,0,9,3,5,2,4,5,7,8,5,7,0,2,8,8,7,9,2,2,7,4,6,5,1,4,9,3,0,3,5,2,2,4,1,5,7,8,1,7,3,9,0,8,8,1,6,9,6,3,2,4,5,9,8,6,10

.text
	.global main

	.equ RCC_AHB2ENR,	0x4002104C
	.equ GPIOB_MODER,	0x48000400
	.equ GPIOB_OSPEEDR,	0x48000408
	.equ GPIOB_BSRR,	0x48000418

	.equ GPIOC_MODER,	0x48000800
	.equ GPIOC_PUPDR,	0x4800080C
	.equ GPIOC_IDR,		0x48000810

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
	bl		Init

ReadBtn:
	ldrh	r7,	[C_IDR]
	lsr		r7, #13
	movs	r8,	#0

	cmp		r7,	r8
	itt	eq
		ldreq	r9, =#400000
		beq		PushBtn
	b		ReadBtn

PushBtn:
	ldr		r7,	[C_IDR]
	lsr		r7, #13
	movs	r8,	#0

	cmp		r7,	r8
	it		eq
		subeq	r9, #1
	cmp		r9, #0
	beq		Init
	cmp		r7,	r8
	beq		PushBtn
	b		DisplayFibonacci

Init:
	bl		DisplayClear

	ldr		lenarr, =len
	ldr		digitarr, =digit
    mov		lenptr, #0
    mov		digitptr, #0

DisplayFibonacci:
	bl		Delay
	ldrb	len, [lenarr, lenptr]
DisplayFibonacciLoop:
	ldrb	digit, [digitarr, digitptr]
	cmp		digit, #10
	beq		tooLarge
	bl		MAX7219Send

	add		digitptr, #1
	subs	len, #1
	bne		DisplayFibonacciLoop

	ldrb	len, [lenarr, lenptr]
	add		lenptr, #1
	b		ReadBtn

tooLarge:
	bl		DisplayClear

	mov		addr, #2
	mov		data, #10
	bl		MAX7219Send

	mov		addr, #1
	mov		data, #1
	bl		MAX7219Send

	b		ReadBtn

DisplayClear:
	push	{lr}
	mov		addr, #8
	mov		data, #15
DisplayClearLoop:
	bl		MAX7219Send

	subs	addr, #1
	bne		DisplayClearLoop

	pop		{lr}
	bx		lr

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

Delay:	// a delay 1sec function
	push	{r0, lr}

	ldr		cntdown, =#900000
DelayLoop:
	subs	cntdown, #2
	bne		DelayLoop
DelayDone:
	pop		{r0, lr}
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
	mov		data, #7
	bl		MAX7219Send

	ldr		addr, =INTENSITY
	mov		data, #4
	bl		MAX7219Send

	ldr		addr, =SHUTDOWN
	mov		data, #1
	bl		MAX7219Send

	pop		{lr}
	bx		lr

GPIO_init:
	// enable AHB2 clock
	movs 	r0, #0x6
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

	// set PC13 as input mode
	ldr		r1, =GPIOC_MODER
	ldr		r0, [r1]
	ldr		r2,	=0xF3FFFFFF
	and		r0, r2
	str		r0, [r1]

	// set PC13 as Pull-up mode
	movs 	r0, #0x4000
	ldr		r1, =GPIOC_PUPDR
	str		r0, [r1]

	ldr		C_IDR, =GPIOC_IDR
	bx		lr
