#ifndef TOOL_H_
#define TOOL_H_
#include "stm32l476xx.h"

#define GPIO_Pin_0  0b0000000000000001
#define GPIO_Pin_1  0b0000000000000010
#define GPIO_Pin_2  0b0000000000000100
#define GPIO_Pin_3  0b0000000000001000
#define GPIO_Pin_4  0b0000000000010000
#define GPIO_Pin_5  0b0000000000100000
#define GPIO_Pin_6  0b0000000001000000
#define GPIO_Pin_7  0b0000000010000000
#define GPIO_Pin_8  0b0000000100000000
#define GPIO_Pin_9  0b0000001000000000
#define GPIO_Pin_10 0b0000010000000000
#define GPIO_Pin_11 0b0000100000000000
#define GPIO_Pin_12 0b0001000000000000
#define GPIO_Pin_13 0b0010000000000000
#define GPIO_Pin_14 0b0100000000000000
#define GPIO_Pin_15 0b1000000000000000

extern void max7219_init();
extern void max7219_send(unsigned char address, unsigned char data);
extern void Delay_1s();

void display(int data) {
	for (int i = 1; i <= 8; i++) {
		if (data > 0)
			max7219_send(i, data%10);
		else {
			if (i == 1)
				max7219_send(i, 0);
			else
				max7219_send(i, 15);
		}
		data /= 10;
	}
}

void user_btn_init() {
	RCC->AHB2ENR	|=	RCC_AHB2SMENR_GPIOCSMEN;
	GPIOC->MODER	&=	~GPIO_MODER_MODE13;		// PC13 for user btn

	GPIOC->OSPEEDR	|=	GPIO_OSPEEDR_OSPEED12_1;
	GPIOC->PUPDR	|=	GPIO_PUPDR_PUPD12_0;
	GPIOC->MODER	&=	GPIO_MODER_MODE12_0;	// PC12 for led
}

void keypad_init() {
	RCC->AHB2ENR   |= 0x00000005;

	GPIOA->MODER   &= 0xfffc03ff;
	GPIOA->PUPDR   &= 0xfffc03ff;
	GPIOA->PUPDR   |= 0x0002a800;
	GPIOA->OSPEEDR &= 0xfffc03ff;
	GPIOA->OSPEEDR |= 0x00015400;

	GPIOC->MODER   &= 0xffffff00;
	GPIOC->MODER   |= 0x00000055;
	GPIOC->PUPDR   &= 0xffffff00;
	GPIOC->PUPDR   |= 0x00000055;
	GPIOC->OSPEEDR &= 0xffffff00;
	GPIOC->OSPEEDR |= 0x00000055;
}

#endif
