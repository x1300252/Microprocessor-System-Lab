#include "stm32l476xx.h"

#define X GPIOC
#define Y GPIOA

#define X0 0x000e0001 // PC0
#define X1 0x000d0002 // PC1
#define X2 0x000b0004 // PC2
#define X3 0x00070008 // PC3
#define Y0 0x00000020 // PA5
#define Y1 0x00000040 // PA6
#define Y2 0x00000080 // PA7
#define Y3 0x00000100 // PA8

unsigned int x_pin = {X0, X1, X2, X3};
unsigned int y_pin = {Y0, Y1, Y2, Y3};

extern void GPIO_init();
extern void max7219_init();
extern void max7219_send(unsigned char address, unsigned char data);

// initial keypad gpio pin, X as output and Y as input
void keypad_init()
{
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

/* TODO: scan keypad value
* return:
*  >=0: key pressed value
*  -1: no key press
*/
char keypad_scan() {
	X->BSRR = X0;

	if (Y->IDR & Y0)
		return 1;
	if (Y->IDR & Y1)
		return 4;
	if (Y->IDR & Y2)
		return 7;
	if (Y->IDR & Y3)
		return 15;

	X->BSRR = X1;

	if (Y->IDR & Y0)
		return 2;
	if (Y->IDR & Y1)
		return 5;
	if (Y->IDR & Y2)
		return 8;
	if (Y->IDR & Y3)
		return 0;

	X->BSRR = X2;

	if (Y->IDR & Y0)
		return 3;
	if (Y->IDR & Y1)
		return 6;
	if (Y->IDR & Y2)
		return 9;
	if (Y->IDR & Y3)
		return 14;

	X->BSRR = X3;

	if (Y->IDR & Y0)
		return 10;
	if (Y->IDR & Y1)
		return 11;
	if (Y->IDR & Y2)
		return 12;
	if (Y->IDR & Y3)
		return 13;

	return -1;
}

int display(int data, int num_digs) {
	int i, neg = (data<0)?1:0;
	int return_val = (data > 99999999 || data < -999999) ? -1 : 0;

	for (i = 1; i <= num_digs; i++) {
		if (data > 0)
			max7219_send(i, data%10);
		else if (data < 0)
			max7219_send(i, 0-data%10);
		else if (neg == 1) {
			max7219_send(i, 10);
			neg = 0;
		}
		else {
			if (i == 1)
				max7219_send(i, 0);
			else
				max7219_send(i, 15);
		}
		data /= 10;
	}
	for (; i <= 8; i++)
		max7219_send(i, 15);

	return return_val;
}

void main(){
	GPIO_init();
	max7219_init();
	keypad_init();

	char input;
	char noinput = -1;
	while(1) {
		input = keypad_scan();
		if (input == noinput)
			display(input, 0);
		else
			display(input, 2);
	}

}
