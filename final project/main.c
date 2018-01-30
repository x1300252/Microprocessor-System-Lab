#include "stm32l476xx.h"
#include "usart.h"
#include <string.h>
#include "wifi.h"
#include "ref.h"
#include "led.h"

char str[100]={0};
int len, cnt=10, weather=0;

void SysTick_Handler(void) {
	display(16, 3, 250);
	if (cnt--)
		return;
	cnt = 10;
	delay_ms(1);
	WiFi_Transmit("AT+CIPSEND=2", strlen("AT+CIPSEND=2"));
	len = WiFi_Receive(&(*str), 100);
	//USART_Transmit(str, len);
	delay_ms(1);
	WiFi_Transmit("??", strlen("??"));
	//delay_ms(1);
	len = WiFi_Receive(&(*str), 100);
	USART_Transmit(str, len);

	weather = str[50]-'0';
	layer = 3;
}

int main() {
	WiFi_init();
	USART_init();
	LED_init();
	layer = 3;
	SysTick_Config(4000000);

	WiFi_Transmit("AT", strlen("AT"));
	len = WiFi_Receive(&(*str), 100);
	USART_Transmit(str, len);

	delay_ms(1);
	WiFi_Transmit("AT+CIPSTART=\"UDP\",\"140.113.168.193\",50020", strlen("AT+CIPSTART=\"UDP\",\"140.113.168.193\",50020"));
	len = WiFi_Receive(&(*str), 100);
	USART_Transmit(str, len);

	while(1) {
		switch(weather){
			case 0:
				wind();
				break;
			case 1:
				thunder();
				break;
			case 2:
				sunny();
				break;
			case 3:
				raining();
				break;
			case 4:
				snowing();
				break;
		}
	}
}


