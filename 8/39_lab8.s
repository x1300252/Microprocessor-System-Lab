    .syntax unified
	.cpu cortex-m4
	.thumb

	// input parameter of max7219_send
	addr	.req r0
	data	.req r1

	// variables for max7219_send
	cmd		.req r0
	BSRR	.req r1
	DIN		.req r2
	CS		.req r3
	CLK		.req r4
	cnt		.req r5

	// variables for Delay_1s
    cntdown .req r0

.text
	.global max7219_init
	.global max7219_send
	.global Delay_1s

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

max7219_send:	// _send a message to max7219
	push	{r0, r1, r2, r3, r4, r5, r6, lr}

	lsl		addr, #8
	add		cmd, addr, data

	ldr		BSRR,	=GPIOB_BSRR

	mov		cnt, #16
max7219_sendLoop:
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
	str		CLK, [BSRR] // clk set 1 to _send 1 bit

	subs 	cnt, #1
	bge		max7219_sendLoop
max7219_sendDone:
	ldr		CS, =CS_RESET
	str  	CS, [BSRR] // cs set 0
	ldr		CS, =CS_SET
	str  	CS, [BSRR] // cs set 1

	pop		{r0, r1, r2, r3, r4, r5, r6, lr}
	bx		lr

DisplayClear:
	push	{r0, r1, lr}

	mov		addr, #8
	mov		data, #15
DisplayClearLoop:
	bl		max7219_send

	subs	addr, #1
	bne		DisplayClearLoop

	pop		{r0, r1, lr}
	bx		lr

Delay_1s:	// a delay 1sec function
	push	{r0, lr}

	ldr		cntdown, =#1000000
DelayLoop:
	subs	cntdown, #1
	bne		DelayLoop
DelayDone:
	pop		{r0, lr}
	bx		lr

max7219_init:
	push	{r0, r1, r2, lr}
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

	ldr		addr, =DECODE_MODE
	mov		data, #0xFF
	bl		max7219_send

	ldr		addr, =DISPLAY_TEST
	mov		data, #0
	bl		max7219_send

	ldr		addr, =SCAN_LIMIT
	mov		data, #7
	bl		max7219_send

	ldr		addr, =INTENSITY
	mov		data, #4
	bl		max7219_send

	ldr		addr, =SHUTDOWN
	mov		data, #1
	bl		max7219_send

	bl		DisplayClear

	pop		{r0, r1, r2, lr}
	bx		lr
