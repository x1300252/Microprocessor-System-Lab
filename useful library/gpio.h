/**
 * @author  Tilen Majerle
 * @email   tilen@majerle.eu
 * @website http://stm32f4-discovery.com
 * @link    http://stm32f4-discovery.com/2015/07/hal-library-1-5-gpio-library-for-stm32fxxx/
 * @version v1.0
 * @ide     Keil uVision
 * @license MIT
 * @brief   GPIO Library for STM32F4xx and STM32F7xx devices
 *
\verbatim
   ----------------------------------------------------------------------
    Copyright (c) 2016 Tilen Majerle

    Permission is hereby granted, free of charge, to any person
    obtaining a copy of this software and associated documentation
    files (the "Software"), to deal in the Software without restriction,
    including without limitation the rights to use, copy, modify, merge,
    publish, distribute, sublicense, and/or sell copies of the Software,
    and to permit persons to whom the Software is furnished to do so,
    subject to the following conditions:

    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
    AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
    OTHER DEALINGS IN THE SOFTWARE.
   ----------------------------------------------------------------------
\endverbatim
 */

#ifndef GPIO_H_
#define GPIO_H_

#define GPIO_SetPinLow(GPIOx, GPIO_Pin)			((GPIOx)->BRR = (1<<(uint32_t)(GPIO_Pin)))
#define GPIO_SetPinHigh(GPIOx, GPIO_Pin)			((GPIOx)->BSRR = (1<<(uint32_t)(GPIO_Pin)))
#define GPIO_GetInputPinValue(GPIOx, GPIO_Pin)	(((GPIOx)->IDR & (1<<(uint32_t)(GPIO_Pin))) == 0 ? 0 : 1)

#define GPIO_Pin_0	((uint32_t) 0)
#define GPIO_Pin_1	((uint32_t) 1)
#define GPIO_Pin_2	((uint32_t) 2)
#define GPIO_Pin_3	((uint32_t) 3)
#define GPIO_Pin_4	((uint32_t) 4)
#define GPIO_Pin_5	((uint32_t) 5)
#define GPIO_Pin_6	((uint32_t) 6)
#define GPIO_Pin_7	((uint32_t) 7)
#define GPIO_Pin_8	((uint32_t) 8)
#define GPIO_Pin_9	((uint32_t) 9)
#define GPIO_Pin_10 ((uint32_t) 10)
#define GPIO_Pin_11 ((uint32_t) 11)
#define GPIO_Pin_12 ((uint32_t) 12)
#define GPIO_Pin_13 ((uint32_t) 13)
#define GPIO_Pin_14	((uint32_t) 14)
#define GPIO_Pin_15	((uint32_t) 15)

typedef enum {
	GPIO_Mode_IN = 0x00,  /*!< GPIO Pin as General Purpose Input */
	GPIO_Mode_OUT = 0x01, /*!< GPIO Pin as General Purpose Output */
	GPIO_Mode_AF = 0x02,  /*!< GPIO Pin as Alternate Function */
	GPIO_Mode_AN = 0x03,  /*!< GPIO Pin as Analog input/output */
} GPIO_Mode_t;

typedef enum {
	GPIO_OType_PP = 0x00, /*!< GPIO Output Type Push-Pull */
	GPIO_OType_OD = 0x01  /*!< GPIO Output Type Open-Drain */
} GPIO_OType_t;

typedef enum {
	GPIO_Speed_Low = 0x00,    /*!< GPIO Speed Low */
	GPIO_Speed_Medium = 0x01, /*!< GPIO Speed Medium */
	GPIO_Speed_Fast = 0x02,   /*!< GPIO Speed Fast, not available on STM32F0xx devices */
	GPIO_Speed_High = 0x03    /*!< GPIO Speed High */
} GPIO_Speed_t;

typedef enum {
	GPIO_PuPd_NOPULL = 0x00, /*!< No pull resistor */
	GPIO_PuPd_UP = 0x01,     /*!< Pull up resistor enabled */
	GPIO_PuPd_DOWN = 0x02    /*!< Pull down resistor enabled */
} GPIO_PuPd_t;

void GPIO_INT_EnableClock(GPIO_TypeDef* GPIOx);
void GPIO_INT_Init(GPIO_TypeDef* GPIOx, uint32_t GPIO_Pin, GPIO_Mode_t GPIO_Mode, GPIO_OType_t GPIO_OType, GPIO_PuPd_t GPIO_PuPd, GPIO_Speed_t GPIO_Speed);
void GPIO_Init(GPIO_TypeDef* GPIOx, uint32_t GPIO_Pin, GPIO_Mode_t GPIO_Mode, GPIO_OType_t GPIO_OType, GPIO_PuPd_t GPIO_PuPd, GPIO_Speed_t GPIO_Speed) {
	/* Check input */
	if (GPIO_Pin == 0x00) {
		return;
	}

	/* Enable clock for GPIO */
	GPIO_INT_EnableClock(GPIOx);

	/* Do initialization */
	GPIO_INT_Init(GPIOx, GPIO_Pin, GPIO_Mode, GPIO_OType, GPIO_PuPd, GPIO_Speed);
}

void GPIO_SetPullResistor(GPIO_TypeDef* GPIOx, uint32_t GPIO_Pin, GPIO_PuPd_t GPIO_PuPd) {
	/* Set GPIO PUPD register */
	GPIOx->PUPDR = (GPIOx->PUPDR & ~(0x03 << (2 * GPIO_Pin))) | ((uint32_t)(GPIO_PuPd << (2 * GPIO_Pin)));

}

void GPIO_SetPinAsInput(GPIO_TypeDef* GPIOx, uint32_t GPIO_Pin) {
	/* Set 00 bits combination for input */
	GPIOx->MODER &= ~(0x03 << (2 * GPIO_Pin));
}

void GPIO_SetPinAsOutput(GPIO_TypeDef* GPIOx, uint32_t GPIO_Pin) {
	/* Set 01 bits combination for output */
	GPIOx->MODER = (GPIOx->MODER & ~(0x03 << (2 * GPIO_Pin))) | (0x01 << (2 * GPIO_Pin));
}

uint32_t GPIO_GetPortSource(GPIO_TypeDef* GPIOx) {
	/* Get port source number */
	/* Offset from GPIOA                       Difference between 2 GPIO addresses */
	return ((uint32_t)GPIOx - (GPIOA_BASE)) / ((GPIOB_BASE) - (GPIOA_BASE));
}


void GPIO_INT_EnableClock(GPIO_TypeDef* GPIOx) {
	RCC->AHB2ENR |= (1 << GPIO_GetPortSource(GPIOx));
}

void GPIO_INT_Init(GPIO_TypeDef* GPIOx, uint32_t GPIO_Pin, GPIO_Mode_t GPIO_Mode, GPIO_OType_t GPIO_OType, GPIO_PuPd_t GPIO_PuPd, GPIO_Speed_t GPIO_Speed) {
	/* Set GPIO PUPD register */
	GPIOx->PUPDR = (GPIOx->PUPDR & ~(0x03 << (2 * GPIO_Pin))) | ((uint32_t)(GPIO_PuPd << (2 * GPIO_Pin)));

	/* Set GPIO MODE register */
	GPIOx->MODER = (GPIOx->MODER & ~((uint32_t)(0x03 << (2 * GPIO_Pin)))) | ((uint32_t)(GPIO_Mode << (2 * GPIO_Pin)));

	/* Set only if output or alternate functions */
	if (GPIO_Mode == GPIO_Mode_OUT || GPIO_Mode == GPIO_Mode_AF) {
		/* Set GPIO OTYPE register */
		GPIOx->OTYPER = (GPIOx->OTYPER & ~(uint32_t)(0x01 << GPIO_Pin)) | ((uint32_t)(GPIO_OType << GPIO_Pin));

		/* Set GPIO OSPEED register */
		GPIOx->OSPEEDR = (GPIOx->OSPEEDR & ~((uint32_t)(0x03 << (2 * GPIO_Pin)))) | ((uint32_t)(GPIO_Speed << (2 * GPIO_Pin)));
	}
}

#endif /* GPIO_H_ */
