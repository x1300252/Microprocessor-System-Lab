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
int arr [16];
int str[160];
int state[160];

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

int press(int ptr, uint16_t pin) {
	int new = 0;
	if (Y->IDR & pin) {
		if (arr[ptr] == 0)
			new = 1;
		arr[ptr] = 1;
	}
	else
		arr[ptr] = 0;
	return new;
}

int keypad_scan() {
	X->BSRR = X0;
	if (press(1, Y0))
		return 1;
	if (press(4, Y1))
		return 4;
	if (press(7, Y2))
		return 7;
	if (press(15, Y3))
		return 15;

	X->BSRR = X1;
	if (press(2, Y0))
		return 2;
	if (press(5, Y1))
		return 5;
	if (press(8, Y2))
		return 8;
	if (press(0, Y3))
		return 0;

	X->BSRR = X2;
	if (press(3, Y0))
		return 3;
	if (press(6, Y1))
		return 6;
	if (press(9, Y2))
		return 9;
	if (press(14, Y3))
		return 14;

	X->BSRR = X3;
	if (press(10, Y0))
		return 10;
	if (press(11, Y1))
		return 11;
	if (press(12, Y2))
		return 12;
	if (press(13, Y3))
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

void cal(int max) {
	int str2[16], state2[16], ptr = 0, i, j;
	for (i = 0; i < max; i++) {
		if (state[i] == 1) {
			if (str[i] == 12) {
				str2[ptr-1] = str2[ptr-1] * str[i+1];
				i++;
			}
			else if (str[i] == 13) {
				str2[ptr-1] = str2[ptr-1] / str[i+1];
				i++;
			}
			else {
				str2[ptr] = str[i];
				state2[ptr] = 1;
				ptr++;
			}
		}
		else {
			str2[ptr] = str[i];
			state2[ptr] = 0;
			ptr++;
		}
	}

	int ans = str2[0];
	for (i = 1; i < ptr; i++) {
		if (state2[i] == 1) {
			if (str2[i] == 10) {
				ans += str2[++i];
			}
			else {
				ans -= str2[++i];
			}
		}
	}

	display(ans, 8);
}

void main(){
	GPIO_init();
	max7219_init();
	keypad_init();

	int input, ope = 0, tmp = 0, ptr=0;
	while(1) {
		input = keypad_scan();
		if (input == -1)
			continue;
		else if (input < 10) {
			tmp = ope;
			tmp *= 10;
			tmp += input;
			if (tmp > 999)
				continue;
			ope *= 10;
			ope += input;
			display(ope, 3);
		}
		else {
			if (input == 15) {
				str[ptr] = ope;
				state[ptr++] = 0;
				cal(ptr);
			}
			else if (input == 14) {
				display(0, 0);
				ptr = 0;
			}
			else {
				display(0, 0);
				if (state[ptr] == 0) {
					str[ptr] = ope;
					state[ptr++] = 0;
					str[ptr] = input;
					state[ptr++] = 1;
				}
			}
			ope = 0;
		}
	}
}
