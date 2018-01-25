#include "stm32l476xx.h"
#include "tool.h"

void SystemClock_Config(int plln, int pllm) {
	RCC->CFGR 		= 	0x00000000;
	RCC->CR			&=	!RCC_CR_PLLON;			// PLL off
	while (RCC->CR & RCC_CR_PLLRDY);			// wait until PLL clock not ready

	RCC->PLLCFGR	= 	RCC_PLLCFGR_PLLSRC_MSI;	// MSI clock selected as PLL clock entry
	RCC->PLLCFGR	|= 	RCC_PLLCFGR_PLLREN;		// PLLCLK output enable
	RCC->PLLCFGR	|= 	RCC_PLLCFGR_PLLR_0;		// PLLR set to 4

	RCC->PLLCFGR 	|= 	plln << 8;
	RCC->PLLCFGR 	|= 	pllm << 4; 				// f(VCO clock) = f(PLL clock input) กั (PLLN / PLLM)
												// f(PLL_R) = f(VCO clock) / PLLR(2)
	RCC->CR 		|= 	RCC_CR_PLLON;			// PLL on
	while (!(RCC->CR & RCC_CR_PLLRDY));			// wait until PLL clock ready

	RCC->CFGR 		= 	RCC_CFGR_SW;			// PLL selected as system clock
}

int switchClk(int state) {
	switch (state) {
		case 0:
			SystemClock_Config(8, 7);		// 1MHz
			break;
		case 1:
			SystemClock_Config(48, 7);		// 6MHz
			break;
		case 2:
			SystemClock_Config(80, 7);		// 10MHz
			break;
		case 3:
			SystemClock_Config(64, 3);		// 16MHz
			break;
		case 4:
			SystemClock_Config(80, 1);		// 40MHz
			break;
		default:
			break;
	}
	return (state+1)%5;
}

int btn=0;
int press() {
	int new = 0;
	if ((GPIOC->IDR & GPIO_Pin_13)>>13) {
		if (btn == 0)
			new = 1;
		btn = 1;
	}
	else
		btn = 0;
	return new;
}

int main(){
	int state = 0;
	SystemClock_Config(8, 7);
	GPIO_init();
	while(1){
		GPIOC->BSRR = (1<<12);
		Delay_1s();
		if (press())
			state = switchClk(state);

		GPIOC->BRR = (1<<12);
		Delay_1s();
		if (press())
			state = switchClk(state);
	}
}
