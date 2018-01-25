#include "stm32l476xx.h"
#include "tool.h"

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

#define DO 261.6
#define RE 293.7
#define MI 329.6
#define FA 349.2
#define SO 392.0
#define LA 440.0
#define SI 493.9
#define HI_DO 523.3

void GPIO_init_AF() {
	RCC->AHB2ENR	|=	RCC_AHB2SMENR_GPIOBSMEN;
	GPIOB->MODER	&=	GPIO_MODER_MODE0_1;
	RCC->APB1ENR1	|=	RCC_APB1ENR1_TIM3EN;	// enable TIM3 timer
	GPIOB->AFR[0]	|=	GPIO_AFRL_AFSEL0_1;		// AF2 for PB0 (TIM3_CH3)
}

void Timer_init() {
	TIM3->CR1		|=	TIM_CR1_DIR;			// counter used as down counter
	TIM3->CR1 		|= 	TIM_CR1_ARPE;			// Auto-reload preload enable
	TIM3->ARR		=	(uint32_t) 100;
	TIM3->EGR 		= 	TIM_EGR_UG;
}

void PWM_channel_init(){
	TIM3->CCMR2 	&= 	0xfffffffc;				// CC3 channel is configured as output
	TIM3->CCMR2 	|= 	(TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3M_1);	// set output compare 3 mode to PWM mode 1
	TIM3->CCMR2 	|= 	TIM_CCMR2_OC3PE;		// enable output compare 3 preload register on TIM3_CCR2
	TIM3->CCER 		|= 	TIM_CCER_CC3E;			// enable compare 3 output
}

void timer_config(int f, int duty) {
	TIM3->PSC 	= 	(uint32_t)(4000000/f/100);
	TIM3->CCR3	= 	duty;
	TIM3->CR1 	|= 	TIM_CR1_CEN;
}

char keypad_scan() {
	X->BSRR = X0;
	if (Y->IDR & Y0) return 1;
	if (Y->IDR & Y1) return 4;
	if (Y->IDR & Y2) return 7;
	if (Y->IDR & Y3) return 15;

	X->BSRR = X1;
	if (Y->IDR & Y0) return 2;
	if (Y->IDR & Y1) return 5;
	if (Y->IDR & Y2) return 8;
	if (Y->IDR & Y3) return 0;

	X->BSRR = X2;
	if (Y->IDR & Y0) return 3;
	if (Y->IDR & Y1) return 6;
	if (Y->IDR & Y2) return 0;
	if (Y->IDR & Y3) return 14;

	return 0;
}

int main() {
	keypad_init();
	GPIO_init_AF();
	Timer_init();
	PWM_channel_init();

	int curbtn=0, prebtn=0, duty = 50, state;
	while (1) {
		prebtn = curbtn;
		curbtn = keypad_scan();

		if (curbtn!=prebtn)
			state = curbtn;
		else
			state = -1;

		switch (state) {
			case -1:
				break;
			case 1:
				timer_config(DO, duty);
				break;
			case 2:
				timer_config(RE, duty);
				break;
			case 3:
				timer_config(MI, duty);
				break;
			case 4:
				timer_config(FA, duty);
				break;
			case 5:
				timer_config(SO, duty);
				break;
			case 6:
				timer_config(LA, duty);
				break;
			case 7:
				timer_config(SI, duty);
				break;
			case 8:
				timer_config(HI_DO, duty);
				break;
			case 15:
				duty = duty == 90 ? duty : duty + 5;
				break;
			case 14:
				duty = duty == 10 ? duty : duty - 5;
				break;
			default:
				TIM3->CR1 &= ~TIM_CR1_CEN;
				break;
		}
	}
}
