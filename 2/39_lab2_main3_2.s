	.syntax unified
	.cpu cortex-m4
	.thumb

.text
    .global main
	.equ N, 50

fib:
    //TODO
	cmp R0, #100	// if N > 100, R4 = -1 and return
	itt gt
		mvngt R4, #0
		bxgt lr

	cmp R0, #1		// if N < 1,   R4 = -1 and return
	itt lt
		mvnlt R4, #0
		bxlt lr

	movs R1, #0	// fib[0]
    movs R2, #1 // fib[1]

loopNextOne:
	sub R0, #1	// R0--
    cbz R0, done	// if get fib[N], done

    adds R3, R1, R2	// fib[N+1] = fib[N] + fib[N-1]

	itt vs			// if overflow,   R4 = -2 and return
    	mvnvs R4, #1
    	bxvs lr

nextOne:
	movs R1, R2		// R1 = fib[N-1]
	movs R2, R3		// R2 = fib[N]
	b loopNextOne

done:
	movs R4, R2	//store result in R4 and return
    bx lr

main:
    movs R0, #N

	bl fib

L: b L
