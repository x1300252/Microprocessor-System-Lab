/*
 * LCD.h
 *
 *  Created on: 2017¦~12¤ë22¤é
 *      Author: x1300252
 */

#ifndef LCD_H_
#define LCD_H_

#include "gpio.h"
#include "ref.h"

#define CMD		1
#define DATA	0

#define LCD_RSPin	GPIO_Pin_3	// PB3
#define LCD_RWPin	GPIO_Pin_4	// PB4
#define LCD_ENPin	GPIO_Pin_5	// PB5
uint32_t LCD_data_pin[8] = 	{GPIO_Pin_6, GPIO_Pin_7, GPIO_Pin_8, GPIO_Pin_9, GPIO_Pin_4, GPIO_Pin_5, GPIO_Pin_6, GPIO_Pin_7};
GPIO_TypeDef* LCD_data_gpio[8] = {GPIOB, GPIOC, GPIOB, GPIOB, GPIOA, GPIOA, GPIOA, GPIOA};

void WriteToLCD(int input, int isCmd) {
	if (isCmd)
		GPIO_SetPinLow(GPIOB, LCD_RSPin);	// instruction I/O
	else
		GPIO_SetPinHigh(GPIOB, LCD_RSPin);	// data I/O

	GPIO_SetPinLow(GPIOB, LCD_RWPin);		// write data to LCD

	for (int i = 0 ; i < 8 ; i++) {
		if (input%2)
			GPIO_SetPinHigh(LCD_data_gpio[i], LCD_data_pin[i]);
		else
			GPIO_SetPinLow(LCD_data_gpio[i], LCD_data_pin[i]);
		input = input >> 1;
	}

	GPIO_SetPinHigh(GPIOB, LCD_ENPin);
	delay_ms(10);
	GPIO_SetPinLow(GPIOB, LCD_ENPin);
	delay_ms(10);
}

void LCD_GPIO_init() {
	for (int i=0; i<8; i++)
		GPIO_Init(LCD_data_gpio[i], LCD_data_pin[i], GPIO_Mode_OUT, GPIO_OType_PP, GPIO_PuPd_UP, GPIO_Speed_Medium);

	GPIO_Init(GPIOB, LCD_RSPin, GPIO_Mode_OUT, GPIO_OType_PP, GPIO_PuPd_UP, GPIO_Speed_Medium);
	GPIO_Init(GPIOB, LCD_RWPin, GPIO_Mode_OUT, GPIO_OType_PP, GPIO_PuPd_UP, GPIO_Speed_Medium);
	GPIO_Init(GPIOB, LCD_ENPin, GPIO_Mode_OUT, GPIO_OType_PP, GPIO_PuPd_UP, GPIO_Speed_Medium);
}

void LCD_init() {
	LCD_GPIO_init();

	WriteToLCD(0x38, CMD); 	// Function Setting
							//	D4=1: 8-bit bus mode
							//	D3=1: 2-line display
							//	D2=0: 5*8dots format display mode
	WriteToLCD(0x06, CMD); 	// Entering Mode:
							//	D2=1: cursor moves to right and DDRAM addr += 1
							//	D1=1: doesn't shift entire display
	WriteToLCD(0x0C, CMD);	// Display on
							//	D2=1: turn entire display
							//	D1=0: cursor disappear
							//	D0=0: blink off
	WriteToLCD(0x01, CMD); 	// Clear Screen
	WriteToLCD(0x80, CMD); 	// Move to top left
}

#endif /* LCD_H_ */
