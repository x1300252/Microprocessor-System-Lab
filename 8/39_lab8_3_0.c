#include "tool.h"
#include "gpio.h"
#include "ds18b20.h"
#include "onewire.h"

OneWire_t TM;

void SysTick_Handler(void) {
	//TODO: Show temperature on 7-seg display
	GPIOC->BSRR = GPIO_Pin_12;
	DS18B20_ConvT(&TM);
	delay_ms(100);
	int data = (DS18B20_Read(&TM)&0x07FF)>>4;
	display(data);
	GPIOC->BRR = GPIO_Pin_12;
}

int btn=1;
int user_press_button() {
	int new = 0;
	if (GPIOC->IDR & GPIO_Pin_13) {
		if (btn == 0)
			new = 1;
		btn = 1;
	}
	else {
		btn = 0;
	}
	return new;
}

void GPIO_init() {
	OneWire_Init(&TM, GPIOC, 11);
	DS18B20_SetResolution(&TM, TM_DS18B20_Resolution_9bits);
	max7219_init();
	user_btn_init();
}

int main() {
	GPIO_init();
	SysTick_Config(8000000);
	while(1){
		if(user_press_button()) {
			if (SysTick->CTRL & 0x00000001) {
				SysTick->CTRL &= 0xFFFFFFFE;
			}
			else {
				SysTick->CTRL |= 0x00000001;
			}
		}
	}
}
