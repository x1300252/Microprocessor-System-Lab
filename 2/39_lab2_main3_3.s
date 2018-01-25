	.syntax unified
	.cpu cortex-m4
	.thumb

.data
	arr1: .byte 0x19, 0x34, 0x14, 0x32, 0x52, 0x23, 0x61, 0x29
	arr2: .byte 0x18, 0x17, 0x33, 0x16, 0xFA, 0x20, 0x55, 0xAC

.text
    .global main

do_sort:
    //TODO
    movs r1, #7 // len-1
    movs r2, #0 // i = 0
    b Loop1

Loop2:
	add r4, r3, #1 // j+1

	ldrb r5, [r0, r3] // arr[j]
	ldrb r6, [r0, r4] // arr[j+1]

	cmp r5, r6
	ittt gt					// if arr[j] > arr[j+1], swap
		movgt r7, r5		// tmp = arr[j]
		strbgt r6, [r0, r3]	// arr[j] = arr[j+1]
		strbgt r7, [r0, r4]	// arr[j+1] = tmp

	add r3, r3, #1 // j += 1

	cmp r3, r8
	ite ge // if len-1-i > j
		addge r2, r2, #1 // true, i++ then goto Loop1
		blt Loop2 // false, continue

Loop1:
	cmp r1, r2
	ittt gt // if len-1 > i
		subgt r8, r1, r2 // len-1-i
		movgt r3, #0 // j = 0
		bgt Loop2
	bx lr

main:
    ldr r0, =arr1
    bl do_sort
    ldr r0, =arr2
    bl do_sort

L: b L
