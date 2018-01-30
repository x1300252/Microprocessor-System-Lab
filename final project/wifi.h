/*
 * wifi.h
 *
 *  Created on: 2018¦~1¤ë14¤é
 *      Author: x1300252
 */

#ifndef WIFI_H_
#define WIFI_H_

#include "stm32l476xx.h"
#include <string.h>
#include "gpio.h"

#define IS_WIFI_TRANS_READY	(USART3->ISR & USART_ISR_TXE) 	// check transmit data register empty
#define IS_WIFI_TRANS_DONE 	(USART3->ISR & USART_ISR_TC)	// check transmission complete
#define IS_WIFI_READ_READY	(USART3->ISR & USART_ISR_RXNE)	// check read data register empty
#define IS_WIFI_READ_DONE	(USART3->ISR & USART_ISR_IDLE)	// check read data register empty

#define WIFI_TX GPIO_Pin_10	// PA2
#define WIFI_RX GPIO_Pin_11	// PA3

void WiFi_GPIO_init() {
	GPIO_Init(GPIOB, WIFI_RX, GPIO_Mode_AF, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_Low);
	GPIO_Init(GPIOB, WIFI_TX, GPIO_Mode_AF, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_Low);
	GPIOB->AFR[1] |= (7 << GPIO_AFRH_AFSEL10_Pos) | (7 << GPIO_AFRH_AFSEL11_Pos);
}

void WiFi_init() {
	WiFi_GPIO_init();

	RCC->APB1ENR1 |= RCC_APB1ENR1_USART3EN;	// enable the clock for USART3

	USART3->CR1 &= ~USART_CR1_M;			// M[1:0] = 00, 1 Start bit, 8 data bits,
	USART3->CR1 &= ~USART_CR1_OVER8;		// Oversampling by 16
	USART3->CR1 &= ~USART_CR1_PCE;			// Parity control disabled
	USART3->CR1 |= USART_CR1_TE;			// enable transmitter
	USART3->CR1 |= USART_CR1_RE;			// enable receiver

	USART3->CR2 &= ~USART_CR2_STOP;			// 1 stop bit

	USART3->CR3 &= ~USART_CR3_CTSE;			// disable the CTS hardware flow control
	USART3->CR3 &= ~USART_CR3_RTSE;			// disable the RTS hardware flow control

	USART3->BRR |= 4000000L / 115200L;		// baud rate = 9600

	USART3->CR2 &= ~(USART_CR2_LINEN | USART_CR2_CLKEN);
	USART3->CR3 &= ~(USART_CR3_SCEN | USART_CR3_HDSEL | USART_CR3_IREN);

	USART3->CR1 |= USART_CR1_UE;			// enable UART
}

void WiFi_Transmit(const char *str, int size) {
	for (int i=0; i<size; i++) {
		while (!IS_WIFI_TRANS_READY);
		USART3->TDR = str[i]&0xff;
	}
	while (!IS_WIFI_TRANS_READY);
	USART3->TDR = '\r'&0xff;
	while (!IS_WIFI_TRANS_READY);
	USART3->TDR = '\n'&0xff;
	while (!IS_WIFI_TRANS_DONE);
}

int WiFi_Receive(char *str, int size) {
	char c;
	int pos = 0, cnt;
/*	while (!IS_WIFI_READ_READY);
	c = USART3->RDR;
	str[pos++]= c;
*/	while(1){
		cnt = 20000;
		while (!IS_WIFI_READ_READY) {
			if(!(cnt--)) {
				str[pos] = '\0';
				return pos;
			}
		}
		c = USART3->RDR;
		str[pos++]= c;
	}
	str[pos] = '\0';
	return pos;
}

#endif /* WIFI_H_ */
