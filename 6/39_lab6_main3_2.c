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

unsigned int x_pin[4] = {X0, X1, X2, X3};
unsigned int y_pin[4] = {Y0, Y1, Y2, Y3};
int arr[15] = {0};

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

int press(int ptr, uint16_t pin) {
	int new = 0;
	if (Y->IDR & pin) {
		if (arr[ptr] == 0)
			new = ptr;
		arr[ptr] = 1;
	}
	else
		arr[ptr] = 0;
	return new;
}

/* TODO: scan keypad value
* return:
*  >=0: key pressed value
*  -1: no key press
*/
char keypad_scan() {
	int set = 0, amt = 0;

	X->BSRR = X0;
	amt += press(1, Y0);
	amt += press(4, Y1);
	amt += press(7, Y2);
	set = press(14, Y3);
	if (set) {
		for (int i = 0; i < 13; i++)
			arr[i] = 0;
		display(0, 0);
		return -1;
	}

	X->BSRR = X1;
	amt += press(2, Y0);
	amt += press(5, Y1);
	amt += press(8, Y2);
	amt += press(0, Y3);

	X->BSRR = X2;
	amt += press(3, Y0);
	amt += press(6, Y1);
	amt += press(9, Y2);
	set = press(14, Y3);
	if (set) {
		for (int i = 0; i < 13; i++)
			arr[i] = 0;
		display(0, 0);
		return -1;
	}

	X->BSRR = X3;
	amt += press(10, Y0);
	amt += press(11, Y1);
	amt += press(12, Y2);
	amt += press(13, Y3);

	return amt;
}

void main(){
	GPIO_init();
	max7219_init();
	keypad_init();

	for (int i = 0; i < 13; i++)
		arr[i] = 0;

	char input , ans = 0;
	char noinput = -1;
	while(1) {
		input = keypad_scan();
		if (input == noinput) {
			display(input, 0);
			ans = 0;
		}
		else {
			if ((ans + input) > 99999999)
				continue;
			ans += input;
			display(ans, 8);
		}
	}

}
