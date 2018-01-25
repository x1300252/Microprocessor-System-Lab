	.syntax unified
	.cpu cortex-m4
	.thumb

.data
	X: .space 4
	Y: .space 4
	Z: .space 4

.text
	.global main
	.equ s, 0x5
	.equ t, 0xA

main:
	ldr  r0, =X
	ldr  r1, =Y

	movs r2, #s
	movs r3, #t

	str r2, [r0] // X = 5
	str r3, [r1] // Y = 10

	muls r2, r2, r3 // X *= 10
	adds r2, r2, r3 // X += Y
	str  r2, [r0]

	ldr  r0, =Z

	subs r1, r3, r2 // Z = Y-X
	str r1, [r0]
