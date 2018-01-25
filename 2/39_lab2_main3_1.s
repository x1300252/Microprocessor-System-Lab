	.syntax unified
	.cpu cortex-m4
	.thumb

.data
    result: .byte 0

.text
    .global main
	.equ X, 0x55AA
	.equ Y, 0xAA55

hamm:
	eor R4, R0, R1 // R4 = X xor Y

loopAddOne:
	cmp R4, #0
	bne addOne	// if R4 != 0, loop
	beq done	// if R4 == 0, done

addOne:
	adds R3, R3, #1 // distance++
	subs R5, R4, #1 // R5 = R4 - 1
	and  R4, R4, R5 // R4 &= R5
	b loopAddOne

done:
	str R3, [R2] // store the result
    bx lr // return

main:
    movw R0, #X // load 16 bit value in the lower half of a register and set the upper half to zero
    movw R1, #Y

    ldr R2, =result
    ldr R3, [R2]

    bl hamm
L: b L
