#include "stm32l476xx.h"
#include "LCD.h"
#include "usart.h"
#include <string.h>

int main (void) {
	USART_init();
	LCD_init();
	char str[40];
	while(1) {
		USART_Receive(&(*str), 40);
		USART_Transmit(str, strlen(str));
		WriteToLCD(0x01, CMD); 	// Clear Screen
		WriteToLCD(0x80, CMD); 	// Move to top left
		for (int i=0; i<strlen(str)-1; i++) {
			if(i==16)
				WriteToLCD(0xC0, CMD);	// new line
			WriteToLCD(str[i], DATA);
		}

	}

	return 0;
}
