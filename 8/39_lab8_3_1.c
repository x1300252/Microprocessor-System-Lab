#include "tool.h"

#define X GPIOC	//output
#define Y GPIOA	//input

#define X0 0x000e0001 // PC0
#define X1 0x000d0002 // PC1
#define X2 0x000b0004 // PC2
#define X3 0x00070008 // PC3
#define Y0 0x00000020 // PA5
#define Y1 0x00000040 // PA6
#define Y2 0x00000080 // PA7
#define Y3 0x00000100 // PA8

int row = 0;
int bsrr[4] = {X0, X1, X2, X3};
char key_value = 0;

void SystemClock_Config(){
	SysTick_Config(400000);
	//Sets the priority of an interrupt
	NVIC_SetPriority(SysTick_IRQn, 0);
}

void EXTI_Setup(){
	//TODO: Setup EXTI interrupt
	SYSCFG->EXTICR[1] = SYSCFG_EXTICR2_EXTI5_PA;
	SYSCFG->EXTICR[1] |= SYSCFG_EXTICR2_EXTI6_PA;
	SYSCFG->EXTICR[1] |= SYSCFG_EXTICR2_EXTI7_PA;
	SYSCFG->EXTICR[2] = SYSCFG_EXTICR3_EXTI8_PA;

	// Interrupt Mask on line x
	EXTI->IMR1 |= EXTI_IMR1_IM5;
	EXTI->IMR1 |= EXTI_IMR1_IM6;
	EXTI->IMR1 |= EXTI_IMR1_IM7;
	EXTI->IMR1 |= EXTI_IMR1_IM8;

	//Falling trigger enabled (for Event and Interrupt) for input line
	EXTI->FTSR1 |= EXTI_FTSR1_FT5;
	EXTI->FTSR1 |= EXTI_FTSR1_FT6;
	EXTI->FTSR1 |= EXTI_FTSR1_FT7;
	EXTI->FTSR1 |= EXTI_FTSR1_FT8;

	// enable interrupts
	asm("cpsie i;");

	//Sets the priority of an interrupt
	NVIC_SetPriority(EXTI9_5_IRQn, 1);

	//Enables a device-specific interrupt in the NVIC interrupt controller
	NVIC_EnableIRQ(EXTI9_5_IRQn);
}

void SysTick_Handler(void) {
	row = (row+1)%4;
	X->BSRR = bsrr[row];
}

void EXTI9_5_IRQHandler(void){
	//TODO: Read the keypad row value
	int press = Y->IDR;
	if (press & Y0) {
		switch(row) {
			case 0: key_value = 1; break;
			case 1: key_value = 2; break;
			case 2: key_value = 3; break;
			case 3: key_value = 10; break;
		}
		EXTI->PR1 |= EXTI_PR1_PIF5;
	}
	else if (press & Y1) {
		switch(row) {
			case 0: key_value = 4; break;
			case 1: key_value = 5; break;
			case 2: key_value = 6; break;
			case 3: key_value = 11; break;
		}
		EXTI->PR1 |= EXTI_PR1_PIF6;
	}
	else if (press & Y2) {
		switch(row) {
			case 0: key_value = 7; break;
			case 1: key_value = 8; break;
			case 2: key_value = 9; break;
			case 3: key_value = 12; break;
		}
		EXTI->PR1 |= EXTI_PR1_PIF7;
	}
	else if (press & Y3) {
		switch(row) {
			case 0: key_value = 15; break;
			case 1: key_value = 0; break;
			case 2: key_value = 14; break;
			case 3: key_value = 13; break;
		}
		EXTI->PR1 |= EXTI_PR1_PIF8;
	}
	//Clears the pending bit of an external interrupt
	NVIC_ClearPendingIRQ(EXTI9_5_IRQn);
}

void GPIO_init() {
	max7219_init();
	keypad_init();
}

int main(){
	GPIO_init();
	EXTI_Setup();
	SystemClock_Config();
	while(1){
		display(key_value);
	}
}
