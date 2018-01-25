#include "stm32l476xx.h"
#include "LCD.h"
#include "usart.h"
#include "ADC.h"
#include <string.h>

int ADC_val=0;
int btn=1;

void btn_init() {
	GPIO_Init(GPIOC, GPIO_Pin_13, GPIO_Mode_IN, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_Low);
}

int press_btn() {
	int new = 0;
	if (GPIO_GetInputPinValue(GPIOC, 13)) {
		if (btn == 0)
			new = 1;
		btn = 1;
	}
	else {
		btn = 0;
	}
	return new;
}

void ADC1_2_IRQHandler (void) {
	ADC_val = ADC1->DR;
	ADC1->ISR |= ADC_ISR_EOC;
}

void SysTick_Handler() {
	ADC1->CR |= ADC_CR_ADSTART; // start adc conversion
}

int main (void) {
	btn_init();
	USART_init();
	ADC_init();

	char str[40];
	while(1) {
		if(press_btn()) {
			int n = ADC_val;
			int len = 1;
			while (n > 9) {
				n /= 10;
				++len;
			}
			str[len] = '\n';
			n = ADC_val;
			for (int i=len-1; i>=0; i--) {
				str[i] = n%10+'0';
				n/=10;
			}

			USART_Transmit(str, len+1);
		}

	}

	return 0;
}
