#ifndef TM_DS18B20_H
#define TM_DS18B20_H

#include "onewire.h"

typedef enum {
	TM_DS18B20_Resolution_9bits = 9,   /*!< DS18B20 9 bits resolution */
	TM_DS18B20_Resolution_10bits = 10, /*!< DS18B20 10 bits resolution */
	TM_DS18B20_Resolution_11bits = 11, /*!< DS18B20 11 bits resolution */
	TM_DS18B20_Resolution_12bits = 12  /*!< DS18B20 12 bits resolution */
} DS18B20_Resolution_t;

#define CONVERT_T 		0x44
#define W_SCRATCHPAD	0x4E
#define R_SCRATCHPAD	0xBE
#define COPY_SCRATCHPAD	0x48

/* Check if the temperature conversion is done or not
 * param:
 *   OneWire: send through this
 * retval:
 *    0 -> OK
 *    1 -> Not yet
 */
uint8_t DS18B20_Done(OneWire_t* OneWire) {
	TM_GPIO_SetPinAsInput(OneWire->GPIOx, OneWire->GPIO_Pin);
	return OneWire_ReadBit(OneWire);
}

/* Set resolution of the DS18B20
 * param:
 *   OneWire: send through this
 *   resolution: set to this resolution
 * retval:
 *    0 -> OK
 *    1 -> Error
 */
uint8_t DS18B20_SetResolution(OneWire_t* OneWire, DS18B20_Resolution_t resolution) {
	// write scratchpad
	OneWire_Reset(OneWire);
	OneWire_SkipROM(OneWire);
	OneWire_WriteByte(OneWire, W_SCRATCHPAD);

	OneWire_WriteByte(OneWire, 100);
	OneWire_WriteByte(OneWire, 0);

	for (int i=0; i<5; i++)
		OneWire_WriteBit(OneWire, 1);
	switch (resolution) {
		case TM_DS18B20_Resolution_9bits:
			OneWire_WriteBit(OneWire, 0);
			OneWire_WriteBit(OneWire, 0);
			break;
		case TM_DS18B20_Resolution_10bits:
			OneWire_WriteBit(OneWire, 1);
			OneWire_WriteBit(OneWire, 0);
			break;
		case TM_DS18B20_Resolution_11bits:
			OneWire_WriteBit(OneWire, 0);
			OneWire_WriteBit(OneWire, 1);
			break;
		case TM_DS18B20_Resolution_12bits:
			OneWire_WriteBit(OneWire, 1);
			OneWire_WriteBit(OneWire, 1);
			break;
	}
	OneWire_WriteBit(OneWire, 0);

	// copy scratchpad
	OneWire_Reset(OneWire);
	OneWire_SkipROM(OneWire);
	OneWire_WriteByte(OneWire, COPY_SCRATCHPAD);
	return 0;
}

/* Send ConvT through OneWire with resolution
 * param:
 *   OneWire: send through this
 *   resolution: temperature resolution
 * retval:
 *    0 -> OK
 *    1 -> Error
 */
int DS18B20_ConvT(OneWire_t* OneWire) {
	OneWire_Reset(OneWire);
	OneWire_SkipROM(OneWire);
	OneWire_WriteByte(OneWire, CONVERT_T);

	return 0;
}

/* Read temperature from OneWire
 * param:
 *   OneWire: send through this
 *   destination: output temperature
 * retval:
 *    0 -> OK
 *    1 -> Error
 */
int DS18B20_Read(OneWire_t* OneWire) {
	int data = 0;
	OneWire_Reset(OneWire);
	OneWire_SkipROM(OneWire);
	OneWire_WriteByte(OneWire, R_SCRATCHPAD);
	data |= OneWire_ReadByte(OneWire);
	data |= OneWire_ReadByte(OneWire)<<8;
	return data;
}

#endif
