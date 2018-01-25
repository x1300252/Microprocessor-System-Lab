/*
 * usart.h
 *
 *  Created on: 2017¦~12¤ë22¤é
 *      Author: x1300252
 */

#ifndef USART_H_
#define USART_H_

#include "stm32l476xx.h"
#include <string.h>
#include "gpio.h"

#define IS_UART_TRANS_READY	(USART1->ISR & USART_ISR_TXE) 	// check transmit data register empty
#define IS_UART_TRANS_DONE 	(USART1->ISR & USART_ISR_TC)	// check transmission complete
#define IS_UART_READ_READY	(USART1->ISR & USART_ISR_RXNE)	// check read data register empty

#define TX GPIO_Pin_9	// PA9
#define RX GPIO_Pin_10	// PA10

void USART_GPIO_init() {
	GPIO_Init(GPIOA, RX, GPIO_Mode_AF, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_Low);
	GPIO_Init(GPIOA, TX, GPIO_Mode_AF, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_Low);
	GPIOA->AFR[1] |= (7 << GPIO_AFRH_AFSEL9_Pos) | (7 << GPIO_AFRH_AFSEL10_Pos);
}

void USART_init() {
	USART_GPIO_init();

	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;	// enable the clock for USART1

	USART1->CR1 &= ~USART_CR1_M;			// M[1:0] = 00, 1 Start bit, 8 data bits,
	USART1->CR1 &= ~USART_CR1_OVER8;		// Oversampling by 16
	USART1->CR1 &= ~USART_CR1_PCE;			// Parity control disabled
	USART1->CR1 |= USART_CR1_TE;			// enable transmitter
	USART1->CR1 |= USART_CR1_RE;			// enable receiver

	USART1->CR2 &= ~USART_CR2_STOP;			// 1 stop bit

	USART1->CR3 &= ~USART_CR3_CTSE;			// disable the CTS hardware flow control
	USART1->CR3 &= ~USART_CR3_RTSE;			// disable the RTS hardware flow control

	USART1->BRR |= 4000000L / 9600L;		// baud rate = 9600

	USART1->CR2 &= ~(USART_CR2_LINEN | USART_CR2_CLKEN);
	USART1->CR3 &= ~(USART_CR3_SCEN | USART_CR3_HDSEL | USART_CR3_IREN);

	USART1->CR1 |= USART_CR1_UE;			// enable UART
}

void USART_Transmit(const char *str, int size) {
	for (int i=0; i<size; i++) {
		while (!IS_UART_TRANS_READY);
		USART1->TDR = str[i]&0xff;
	}
	while (!IS_UART_TRANS_DONE);
}

void USART_Receive(char *str, int size) {
	char c;
	int pos = 0;
	do {
		while (!IS_UART_READ_READY);
		c = USART1->RDR;
		str[pos]= c;
		pos++;
	} while(c!='\n');
	str[pos] = '\0';
}

#endif /* USART_H_ */
