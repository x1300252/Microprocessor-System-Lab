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
	atoi0: .word 0
	atoi1: .word 0
	atoi2: .word 0
	atoi3: .word 0
	atoi4: .word 0
	atoi5: .word 0
	atoi6: .word 0
	atoi7: .word 0
	atoi8: .word 0
	len: .byte 0

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

	str: .asciz  "5 2 37 99 35"
	.align 4
main:
	bl		GPIO_init
    bl		max7219_init
	bl		DisplayClear

	ldr r0, =str
	movs r1, #0 // ptr = 0
	ldr r3, =atoi0
	movs r9, #0

	b atoi_outer_loop

atoi_outer_loop:
	ldrb r2, [r0, r1] // r2 = str[ptr]

	cmp r2, #32 // if (str[ptr] == ' ')
	itt eq
		addeq r1, #1 // ptr++
		beq atoi_outer_loop

	cmp r2, #0 // if (str[ptr] == '\0') done
	it eq
		beq do_sort

	cmp r2, #48 // if str[ptr] is a num
	it ge
		blge atoi

	b atoi_outer_loop

atoi: // a ¡§convert string to integer¡¨ function
	movs r4, #0 // ans = 0
	movs r5, #10

atoiLoop:
	muls r4, r5 // ans *= 10
	subs r2, #48
	adds r4, r2 // ans += str[ptr] - '0'

	adds r1, #1 // ptr++
	ldrb r2, [r0, r1] // r2 = str[ptr]

	cmp r2, #32
	beq atoiDone // if (str[ptr] != ' ')
	cmp r2, #0
	beq atoiDone // else if (str[ptr] != '\0')

	b atoiLoop

atoiDone:
	str r4, [r3]
	add r3, #4
	add r9, #1

	bx lr // else return

do_sort:
    //TODO
    ldr r0, =atoi0
    subs r1, r9, #1 // len-1
    movs r2, #4
    mul r1, r1, r2
    movs r2, #0 // i = 0
    b Loop1

Loop2:
	add r4, r3, #4 // j+1

	ldr r5, [r0, r3] // arr[j]
	ldr r6, [r0, r4] // arr[j+1]

	cmp r5, r6
	ittt gt					// if arr[j] > arr[j+1], swap
		movgt r7, r5		// tmp = arr[j]
		strgt r6, [r0, r3]	// arr[j] = arr[j+1]
		strgt r7, [r0, r4]	// arr[j+1] = tmp

	add r3, r3, #4 // j += 1

	cmp r3, r8
	ite ge // if len-1-i > j
		addge r2, r2, #4 // true, i++ then goto Loop1
		blt Loop2 // false, continue

Loop1:
	cmp r1, r2
	ittt gt // if len-1 > i
		subgt r8, r1, r2 // len-1-i
		movgt r3, #0 // j = 0
		bgt Loop2


Display:
	movs r0, #1
	ldr r7, =atoi0
	movs r8, 0
	movs r2, #10

Block1:
	cmp r9, r8
	it le
		ble program_end

	bl Delay
	ldr r4, [r7]

Loop:
	sdiv r5, r4, r2
	mul r6, r5, r2
	subs r1, r4, r6

	bl MAX7219Send

	cmp r5, #0
	ittt ne
		movne r4, r5
		addne r0, #1
		bne Loop

	movs r0, #1
	adds r8, #1
	adds r7, #4
	b Block1

program_end:
	b program_end

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

	ldr		cntdown, =#2700000
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

	bx		lr

