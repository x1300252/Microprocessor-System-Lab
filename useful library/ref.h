#ifndef REF_H_
#define REF_H_

//static inline void delay_us (int micros) {
//	/* Go to clock cycles */
//	micros *= (4000000 / 1000000) / 5;
//
//	/* Wait till done */
//	while (micros--);
//
//}

static inline void delay_us(int n){
	asm("push {r0}\r\n"
			"mov r0, r0\r\n"
			"LOOP_US:\r\n"
			"nop\r\n"
			"subs r0, #1\r\n"
			"BGT LOOP_US\r\n"
			"POP {r0}\r\n"
			:: "r" (n));
}

static inline void delay_ms(int n){
	asm("push {r0}\r\n"
			"mov r0, %0\r\n"
			"LOOP:\r\n"
			"subs r0, #1\r\n"
			"BGT LOOP\r\n"
			"POP {r0}\r\n"
			:: "r" (n*1333));
}

#endif /* REF_H_ */
