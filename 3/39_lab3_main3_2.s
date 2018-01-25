	.syntax unified
	.cpu cortex-m4
	.thumb

	init_sp .req r5
	maxi_sp .req r6
	exec_sp .req r3

.data
	result: .word  0
	max_size:  .word  0

.text
	.global main
	m: .word 5
	n: .word 1

main:
	ldr r1, =m
	ldr r0, [r1]
	ldr r2, =n
	ldr r1, [r2]

	mov init_sp, sp
	mov maxi_sp, sp

	push {r0, r1, lr}
	bl GCD

	ldr r2, =result
	str r0, [r2]

	ldr r2, =max_size
	sub r1, init_sp, maxi_sp
	str r1, [r2]

L:
	b L

GCD:
	mov exec_sp, sp
	cmp exec_sp, maxi_sp
	it lt
		movlt maxi_sp, exec_sp

	pop {r0, r1}

	cmp r0, #0 // if (a==0) return b
	itt eq
		moveq r0, r1
		bxeq lr

	cmp r1, #0 // if (b==0) return a
	it eq
		bxeq lr

	and r2, r0, #1
	and r3, r1, #1

	ands r4, r2, r3
	cbnz r4, a_odd_b_odd

	orrs	r4,	r2, r3
	cbz r4, a_even_b_even

	cmp r2, #1
	beq a_odd_b_even

	cmp r3, #1
	beq a_even_b_odd

a_even_b_even:
	lsr r0, r0, #1 // r0 = r0 >> 1
	lsr r1, r1, #1 // r1 = r1 >> 1
	push {r0, r1, lr}
	bl GCD

	lsl r0, r0, #1 // return 2*result
	pop {lr}
	bx lr

a_odd_b_even:
	lsr r1, r1, #1 // r0 = r0 >> 1
	push {r0, r1, lr}
	bl GCD

	pop {lr}
	bx lr // return gcd(a, b>>1)

a_even_b_odd:
	lsr r0, r0, #1 // r0 = r0 >> 1
	push {r0, r1, lr}
	bl GCD

	pop {lr}
	bx lr // return gcd(a>>1, b)

a_odd_b_odd:
	cmp r0, r1
	ittt lt	// if (a < b) swap
		movlt r4, r0
		movlt r0, r1
		movlt r1, r4

	sub r0, r1

	push {r0, r1, lr}
	bl GCD

	pop {lr}
	bx lr // return gcd(|a-b|, min(a, b))
