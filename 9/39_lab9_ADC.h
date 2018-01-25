/*
 * ADC.h
 *
 *  Created on: 2017¦~12¤ë25¤é
 *      Author: x1300252
 */

#ifndef ADC_H_
#define ADC_H_

#include "stm32l476xx.h"
#include "gpio.h"

void ADC_GPIO_init() {
	GPIO_Init(GPIOB, GPIO_Pin_1, GPIO_Mode_AN, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_Low);
	GPIOB->ASCR |= 1<<1;	// Connect analog switch to the ADC input
}

void ADC_init() {
	SysTick_Config(10000);

	ADC_GPIO_init ();

	RCC->AHB2ENR |= RCC_AHB2ENR_ADCEN;					// ADC clock enabled
	ADC123_COMMON->CCR |= 0x01 << ADC_CCR_CKMODE_Pos;	//  HCLK/1 (Synchronous clock mode)

	ADC1->CFGR |= 0b00 << ADC_CFGR_RES_Pos;		// 12bit resolution
	ADC1->CFGR &= ~ADC_CFGR_ALIGN;				// right alignment

	ADC1->SQR1 |= 0 << ADC_SQR1_L_Pos;			// Regular channel sequence length: 1 conversion
	ADC1->SQR1 |= 16 << ADC_SQR1_SQ1_Pos;		// 1st conversion in regular sequence

	ADC1->SMPR2 |= 2 << ADC_SMPR2_SMP16_Pos;	// the sampling time: 12.5 ADC clock cycles

	ADC1->CR &= ~ADC_CR_DEEPPWD;	// exit the deep-power-down mode
	ADC1->CR |= ADC_CR_ADVREGEN;	// enable the ADC voltage regulator
	delay_ms (1);

	ADC1->IER |= ADC_IER_EOCIE;					//  End of regular conversion interrupt enable
	NVIC_EnableIRQ (ADC1_2_IRQn);

	ADC1->CR |= ADC_CR_ADEN;					// enable the ADC (ADCx_CR)
	while (!(ADC1->ISR & ADC_ISR_ADRDY));
}

#endif /* ADC_H_ */
