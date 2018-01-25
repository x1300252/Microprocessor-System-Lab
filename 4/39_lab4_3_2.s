	.syntax unified
	.cpu cortex-m4
	.thumb

	LED		.req	r0
	B_ODR	.req 	r1
	C_IDR	.req 	r6
	cnt		.req	r3
	dir		.req	r4
	pause	.req	r5

.data
	leds: .byte 115

.text
	.global main
	.equ RCC_AHB2ENR,	0x4002104C
	.equ GPIOB_MODER,	0x48000400
	.equ GPIOB_OTYPER,	0x48000404
	.equ GPIOB_OSPEEDR,	0x48000408
	.equ GPIOB_PUPDR,	0x4800040C
	.equ GPIOB_ODR,		0x48000414

	.equ GPIOC_MODER,	0x48000800
	.equ GPIOC_PUPDR,	0x4800080C
	.equ GPIOC_IDR,		0x48000810

main:
	bl		GPIO_init

	ldr		r0, =leds
	ldr		r3, [LED]
	strh	r3, [B_ODR]

	movs	cnt,	#0
	movs	dir,	#1
	movs	pause,	#0

	push	{cnt, dir}

Loop:
	//TODO: Write the display pattern into leds variable
	bl		Delay

	cmp 	pause, #0
	it	eq
		bleq	DisplayLED

	b		Loop

GPIO_init: // Initial LED GPIO pins as output
	// enable AHB2 clock
	movs 	r0, #0x6
	ldr		r1, =RCC_AHB2ENR
	str		r0, [r1]

	// set PB3, PB4, PB5, PB6 as output mode
	movs	r0, #0x1540
	ldr		r1, =GPIOB_MODER
	ldr		r2, [r1]
	and		r2, #0xFFFFC03F
	orrs	r2, r2, r0
	str		r2, [r1]

	// set PB3, PB4, PB5, PB6 as Pull-up mode
	movs 	r0, #0x1540
	ldr		r1, =GPIOB_PUPDR
	str		r0, [r1]

	// set PB3, PB4, PB5, PB6 as high speed mode
	movs 	r0, #0x2A80
	ldr		r1, =GPIOB_OSPEEDR
	strh	r0, [r1]

	// set PC13 as Pull-up mode
	movs 	r0, #0x4000
	ldr		r1, =GPIOC_PUPDR
	str		r0, [r1]

	// set PC13 as input mode
	ldr		r1, =GPIOC_MODER
	ldr		r0, [r1]
	ldr		r2,	=0xF3FFFFFF
	and		r0, r2
	str		r0, [r1]

	ldr		B_ODR, =GPIOB_ODR
	ldr		C_IDR, =GPIOC_IDR

	bx		lr

DisplayLED:
	pop	{cnt, dir}
	ldr	r2, [LED]

	cmp	cnt, #0
	it eq
		moveq	dir,	#1

	cmp cnt, #4
	it eq
		moveq	dir, 	#0

	cmp dir, #1
	ittt eq
		lsleq	r2, r2, #1
		orreq	r2, r2, #1
		addeq	cnt,	#1

	cmp dir, #0
	itt eq
		lsreq	r2, r2, #1
		subeq	cnt,	#1

	strh	r2, [B_ODR]
	str		r2, [LED]

	push	{cnt, dir}
	bx		lr

Delay:	// Write a delay 1sec function
	ldr		r3, =#2000
DelayL1:
	push	{r3, lr}
	bl		ReadBtn
	pop		{r3, lr}
DelayL2:
	subs	r4, #2
	bne		DelayL2
	subs	r3, #2
	bne		DelayL1
DelayDone:
	bx		lr

ReadBtn:
	ldrh	r3,	[C_IDR]
	lsr		r3, #13
	movs	r4,	#0

	cmp		r3,	r4
	itt	eq
		ldreq	r7, =#800
		beq		PushBtn
	itt	ne
		ldrne	r4, =#2000
		bxne	lr

PushBtn:
	subs	r7, #2
	bne		PushBtn

	ldr		r3,	[C_IDR]
	lsr		r3, #13
	movs	r4,	#0

	cmp		r3,	r4
	it	eq
		mvneq	pause, pause
	ldr		r4, =#2000
	bx		lr
