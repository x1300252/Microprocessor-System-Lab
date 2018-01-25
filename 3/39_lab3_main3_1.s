	.syntax unified
	.cpu cortex-m4
	.thumb

.data
	user_stack: .zero 128
	expr_result: .word  0

.text
	.global main
	postfix_expr: .asciz  "-5 200 -40 + 2 - -"
	.align 4
main:
	ldr r0, =expr_result
	msr msp, r0 // let sp point to the end of user_stack

	ldr r0, =postfix_expr

	movs r1, #0 // ptr = 0

	b calculate

calculate:
	ldrb r2, [r0, r1] // r2 = str[ptr]

	cmp r2, #32 // if (str[ptr] == ' ')
	itt eq
		addeq r1, #1 // ptr++
		beq calculate

	cmp r2, #0 // if (str[ptr] == '\0') done
	itttt eq
		ldreq r5, =expr_result
		popeq {r4}
		streq r4, [r5]
		beq program_end

	cmp r2, #43 // if (str[ptr] == '+')
	itttt eq
		popeq {r3, r4}
		addeq r3, r4
		pusheq {r3}
		addeq r1, #1 // ptr++

	cmp r2, #45 // if (str[ptr] == '-')
	itt eq
		addeq r1, #1 // ptr++
		bleq subCode

	cmp r2, #48 // if str[ptr] is a num
	itt ge
		movge r3, #0 // negFlag = 0
		blge atoi

	b calculate

subCode:
	mov r6, lr
	ldrb r2, [r0, r1] // r2 = str[ptr]

	cmp r2, #32 // if (str[ptr] == ' ')
	itttt eq
		popeq {r3, r4}
		subeq r4, r3
		pusheq {r4}
		addeq r1, #1 // ptr++

	cmp r2, #0 // if (str[ptr] == '\0')
	itttt eq
		popeq {r3, r4}
		subeq r4, r3
		ldreq r5, =expr_result
		streq r4, [r5]
	beq program_end

	cmp r2, #48 // if str[ptr] is a num
	itt ge
		movge r3, #1 // negFlag = 1
		blge atoi

	bx r6

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
	cmp r3, #1
	ittte eq // if ans is a negative value
		moveq r3, #0
		subeq r3, r4
		pusheq {r3}
		pushne {r4}

	bx lr // else return

program_end:
	b program_end
