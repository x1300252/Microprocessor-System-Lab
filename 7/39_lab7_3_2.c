#include "stm32l476xx.h"
#include "tool.h"

#define TIME_SEC 123.45

void display(int data, int num_digs) {
	int i, dig3;
	for (i = 1; i <= num_digs; i++) {
		if (data > 0)
			max7219_send(i, data%10);
		else {
			if (i<=3)
				max7219_send(i, 0);
			else
				max7219_send(i, 15);
		}
		if (i==3)
			dig3 = data%10 + 128;
		data /= 10;
	}
	max7219_send(3, dig3);
}

void Timer_init() {
	RCC->APB1ENR1 	|= 	RCC_APB1ENR1_TIM2EN;
	TIM2->ARR 		= 	(uint32_t) 100;				// reload value
	TIM2->PSC 		= 	(uint32_t) 39999;			// prescaler
	TIM2->EGR 		= 	TIM_EGR_UG;					// reinitialize the counter
}

void Timer_start() {
	TIM2->CR1 		|=	TIM_CR1_CEN;				// counter enable
	display(0, 8);
}

int main() {
	//GPIO_init();
	max7219_init();
	Timer_init();
	Timer_start();

	int time_sec = TIME_SEC*100;
	if (time_sec>99999999 || time_sec == 0) {
		TIM2->CR1 &= ~TIM_CR1_CEN;
		return 0;
	}

	int time_int = 0, time_float, time_now = 0, update = 0;
	while(1) {
		time_float = TIM2->CNT;

		if (time_float==0 && update==1) {
			time_int++;
			update = 0;
		}
		else if (time_float>0)
			update = 1;

		time_now = time_int*100 + time_float;

		display(time_now, 8);

		if (time_now == time_sec) {
			TIM2->CR1 &= ~TIM_CR1_CEN;
			return 0;
		}
	}
}
