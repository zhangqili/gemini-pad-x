/*
 * analog.h
 *
 *  Created on: 2023年5月21日
 *      Author: xq123
 */
#include "keyboard.h"
#include "lefl.h"

#ifndef ANALOG_H_
#define ANALOG_H_

#define Analog_Clean() (ADC_Conversion_Count=0)

#define ANALOG_INIT_SCAN_COUNT 32768

extern uint8_t ADC_Conversion_Count;
extern int16_t ADC_Values[ADVANCED_KEY_NUM][64];

extern int16_t ADC_Value_List[ADVANCED_KEY_NUM];
extern uint16_t ADC_Buffer[ADVANCED_KEY_NUM];

extern float ADC_Sums[ADVANCED_KEY_NUM];
extern float ADC_Averages[ADVANCED_KEY_NUM];

extern float Analog_Values[ADVANCED_KEY_NUM];

void Analog_Scan();
void Analog_Init();
void Analog_Average();
void Analog_Check();
void Analog_Recovery();
void Analog_Save();

#endif /* ANALOG_H_ */
