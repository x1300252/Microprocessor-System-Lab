	.syntax unified
	.cpu cortex-m4
	.thumb

	cnt	.req	r5
	dir	.req	r6
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

main:
	bl		GPIO_init

	ldr		r0, =leds
	ldr	r2, [r0]
	strh	r2, [r1]

	movs	cnt,	#0
	movs	dir,	#1

Loop:
	//TODO: Write the display pattern into leds variable
	bl	Delay
	bl	DisplayLED

	b	Loop

GPIO_init: // Initial LED GPIO pins as output
	// enable AHB2 clock
	movs 	r0, #0x2
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

	ldr		r1, =GPIOB_ODR

	bx		lr

DisplayLED:
	ldr	r2, [r0]

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

	strh	r2, [r1]
	str		r2, [r0]

	bx		lr

Delay:	// Write a delay 1sec function
	ldr		r3, =#2700000
DelayL1:
	subs	r3, #2
	bne		DelayL1
DelayDone:
	bx		lr
