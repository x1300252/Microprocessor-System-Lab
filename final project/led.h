/*
 * led.h
 *
 *  Created on: 2018¦~1¤ë21¤é
 *      Author: x1300252
 */

#ifndef LED_H_
#define LED_H_
#include "gpio.h"

int layer;

void LED_init() {
	GPIO_Init(GPIOA, 4, GPIO_Mode_OUT, GPIO_OType_PP, GPIO_PuPd_UP, GPIO_Speed_Medium);
	GPIO_Init(GPIOA, 5, GPIO_Mode_OUT, GPIO_OType_PP, GPIO_PuPd_UP, GPIO_Speed_Medium);
	GPIO_Init(GPIOA, 6, GPIO_Mode_OUT, GPIO_OType_PP, GPIO_PuPd_UP, GPIO_Speed_Medium);
	GPIO_Init(GPIOA, 7, GPIO_Mode_OUT, GPIO_OType_PP, GPIO_PuPd_UP, GPIO_Speed_Medium);
	GPIO_Init(GPIOA, 8, GPIO_Mode_OUT, GPIO_OType_PP, GPIO_PuPd_UP, GPIO_Speed_Medium);

	GPIO_Init(GPIOB, 3, GPIO_Mode_OUT, GPIO_OType_PP, GPIO_PuPd_UP, GPIO_Speed_Medium);
	GPIO_Init(GPIOB, 4, GPIO_Mode_OUT, GPIO_OType_PP, GPIO_PuPd_UP, GPIO_Speed_Medium);
	GPIO_Init(GPIOB, 5, GPIO_Mode_OUT, GPIO_OType_PP, GPIO_PuPd_UP, GPIO_Speed_Medium);
	GPIO_Init(GPIOB, 9, GPIO_Mode_OUT, GPIO_OType_PP, GPIO_PuPd_UP, GPIO_Speed_Medium);
}

void display(int pin, int layer, int sec) {
	switch(layer) {
		case 0:
			GPIO_SetPinHigh(GPIOB, 3);
			GPIO_SetPinLow(GPIOB, 4);
			GPIO_SetPinLow(GPIOB, 5);
			GPIO_SetPinLow(GPIOB, 9);
			break;
		case 1:
			GPIO_SetPinLow(GPIOB, 3);
			GPIO_SetPinHigh(GPIOB, 4);
			GPIO_SetPinLow(GPIOB, 5);
			GPIO_SetPinLow(GPIOB, 9);
			break;
		case 2:
			GPIO_SetPinLow(GPIOB, 3);
			GPIO_SetPinLow(GPIOB, 4);
			GPIO_SetPinHigh(GPIOB, 5);
			GPIO_SetPinLow(GPIOB, 9);
			break;
		case 3:
			GPIO_SetPinLow(GPIOB, 3);
			GPIO_SetPinLow(GPIOB, 4);
			GPIO_SetPinLow(GPIOB, 5);
			GPIO_SetPinHigh(GPIOB, 9);
			break;
	}
	GPIOA->ODR = pin<<4;
	delay_us(sec);
}

void raining() {
	int a=80;
	while(a--) {
		for (int i=0; i<16; i++)
			display(i, 3, 250);
		display(0, layer, 250);
		display(2, layer, 250);
		display(5, layer, 250);
		display(7, layer, 250);
		display(8, layer, 250);
		display(10, layer, 250);
		display(11, layer, 250);
		display(13, layer, 250);
	}
	layer = (layer)?(layer-1):3;
}

void snowing() {
	int a=80;
	while(a--) {
		for (int i=0; i<16; i++)
			display(i, 0, 250);
		display(0, layer, 250);
		display(2, layer, 250);
		display(5, layer, 250);
		display(7, layer, 250);
		display(8, layer, 250);
		display(10, layer, 250);
		display(11, layer, 250);
		display(13, layer, 250);
	}
	layer = (layer)?(layer-1):3;
}

void sunny() {
	int a=200;
	while(a--) {
		display(5, 2, 250);
		display(6, 2, 250);
		display(9, 2, 250);
		display(10, 2, 250);
		display(5, 1, 250);
		display(6, 1, 250);
		display(9, 1, 250);
		display(10, 1, 250);
		switch(layer) {
			case 0:
			case 2:
				display(7, 3, 250);
				display(13, 3, 250);
				display(1, 3, 250);
				display(8, 3, 250);
				display(0, 2, 250);
				display(14, 1, 250);
				display(7, 0, 250);
				display(13, 0, 250);
				display(1, 0, 250);
				display(8, 0, 250);
				break;
			case 1:
			case 3:
				display(0, 3, 250);
				display(12, 3, 250);
				display(3, 3, 250);
				display(15, 3, 250);
				display(0, 1, 250);
				display(14, 2, 250);
				display(0, 0, 250);
				display(12, 0, 250);
				display(3, 0, 250);
				display(15, 0, 250);
				break;
		}
	}
	layer = (layer)?(layer-1):3;
}
void thunder() {
	int a=5;
	while(a--) {
		for (int i=0; i<4; i++)
			for (int j=0; j<16; j++)
				display(j, i, 250);
	}
	display(16, 3, 250);
	delay_ms(100);
	a=5;
	while(a--) {
		for (int i=0; i<4; i++)
			for (int j=0; j<16; j++)
				display(j, i, 250);
	}
	display(16, 3, 250);
	delay_ms(100);
	a=10;
	while(a--) {
		for (int i=0; i<4; i++)
			for (int j=0; j<16; j++)
				display(j, i, 250);
	}
	display(16, 3, 250);
	delay_ms(3000);
}
void wind() {
	for (int i=0; i<4; i++) {
		display(0, i, 15000);
		display(1, i, 15000);
		display(2, i, 15000);
		display(3, i, 15000);
		display(7, i, 15000);
		display(15, i, 15000);
		display(11, i, 15000);
		display(12, i, 15000);
		display(13, i, 15000);
		display(14, i, 15000);
		display(8, i, 15000);
		display(4, i, 15000);
	}
	display(16, 3, 250);
	delay_ms(3000);
}
#endif /* LED_H_ */
