	.syntax unified
	.cpu cortex-m4
	.thumb

	B_ODR	.req 	r0
	C_IDR	.req 	r1
	blink	.req	r2

.data
	anspswd:	.byte	0b1100

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
	movs	r3, #120
	strh	r3, [B_ODR]
	ldrh	r3, [C_IDR]

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
		bne		ReadBtn

PushBtn:
	subs	r7, #2
	bne		PushBtn

	ldr		r3,	[C_IDR]
	lsr		r3, #13
	movs	r4,	#0

	cmp		r3,	r4
	it	ne
		bne	ReadBtn

Check:
	mov		r4, #0x000f
	ldr		r3,	[C_IDR]
	ands	r3, r4
	ldr		r5, =anspswd
	ldr		r4, [r5]

	cmp r3, r4
	ite	eq
		moveq	blink, #3
		movne	blink, #1

BlinkLED:
	movs	r4,	#0
	strh	r4,	[B_ODR]
	bl		Delay

	movs	r4,	#120
	strh	r4,	[B_ODR]
	bl		Delay

	subs	blink, #1
	cmp		blink, #0
	beq		ReadBtn
	bne		BlinkLED

Delay:
	ldr		r5, =#2700000
DelayL1:
	subs	r5, #8
	bne		DelayL1
	bx		lr

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
	orr		r2, r2, r0
	str		r2, [r1]

	// set PB3, PB4, PB5, PB6 as Pull-up mode
	movs 	r0, #0x1540
	ldr		r1, =GPIOB_PUPDR
	str		r0, [r1]

	// set PB3, PB4, PB5, PB6 as high speed mode
	movs 	r0, #0x2A80
	ldr		r1, =GPIOB_OSPEEDR
	strh	r0, [r1]

	// set PC13 as input mode
	ldr		r1, =GPIOC_MODER
	ldr		r0, [r1]
	ldr		r2,	=0xF3FFFF00
	and		r0, r2
	str		r0, [r1]

	ldr  r1, =GPIOC_PUPDR
	ldr  r0, [r1]

	ldr  r2, =0xF3FFFF00
	and	 r0, r2
	ldr  r2, =0x55
	orr  r0, r2
	str  r0, [r1]

	ldr		B_ODR, =GPIOB_ODR
	ldr		C_IDR, =GPIOC_IDR

	bx		lr
