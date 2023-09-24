/*
 * analog.c
 *
 *  Created on: 2023年5月21日
 *      Author: xq123
 */
#include "main.h"
#include "usart.h"
#include "stdlib.h"
#include "stdio.h"
#include "adc.h"
#include "dma.h"
#include "gpio.h"
#include "analog.h"
#include "rgb.h"
#include "flash_address.h"
#include "communication.h"
#include "MB85RC16.h"
#include "lefl.h"

uint8_t ADC_Conversion_Count=0;
int16_t ADC_Values[ADVANCED_KEY_NUM][64];

int16_t ADC_Value_List[ADVANCED_KEY_NUM]={0};
uint16_t ADC_Buffer[ADVANCED_KEY_NUM];
uint8_t ADC_Conversion_Flag=0;

float ADC_Sums[ADVANCED_KEY_NUM]={0.0};
float ADC_Averages[ADVANCED_KEY_NUM]={0.0};

float Analog_Values[ADVANCED_KEY_NUM]={0.0};



void Analog_Init()
{
    RGB_Flash();
    for (uint16_t i = 0; i < ANALOG_INIT_SCAN_COUNT; i++)
    {
        for (uint8_t j = 0; j < ADVANCED_KEY_NUM; j++)
        {
            HAL_ADC_Start(&hadc1);
            HAL_ADC_PollForConversion(&hadc1,1);
            ADC_Sums[j]+=HAL_ADC_GetValue(&hadc1);
        }
    }

    for (uint8_t i = 0; i < ADVANCED_KEY_NUM; i++)
    {
        Keyboard_AdvancedKeys[i].upper_bound=ADC_Sums[i]/(float)ANALOG_INIT_SCAN_COUNT;
        ADC_Sums[i]=0.0;
    }

    Communication_Add8(USART1, PROTOCOL_CMD,CMD_CALIBRATION_DOWN);
    Communication_USART1_Transmit();
    RGB_Colors[0].g=127;
    RGB_Colors[1].b=127;
    RGB_Colors[2].r=127;
    RGB_Colors[3].r=127;
    RGB_Colors[3].g=127;
    RGB_Set(RGB_Colors[0].r,RGB_Colors[0].g,RGB_Colors[0].b,0);
    RGB_Set(RGB_Colors[1].r,RGB_Colors[1].g,RGB_Colors[1].b,1);
    RGB_Set(RGB_Colors[2].r,RGB_Colors[2].g,RGB_Colors[2].b,2);
    RGB_Set(RGB_Colors[3].r,RGB_Colors[3].g,RGB_Colors[3].b,3);
    HAL_Delay(1000);
    for (uint16_t i = 0; i < ANALOG_INIT_SCAN_COUNT; i++)
    {
        for (uint8_t j = 0; j < ADVANCED_KEY_NUM; j++)
        {
            HAL_ADC_Start(&hadc1);
            HAL_ADC_PollForConversion(&hadc1,1);
            ADC_Sums[j]+=HAL_ADC_GetValue(&hadc1);
        }
    }
    for (uint8_t i = 0; i < ADVANCED_KEY_NUM; i++)
    {
        Keyboard_AdvancedKeys[(i+3)%ADVANCED_KEY_NUM].lower_bound=ADC_Sums[i]/(float)ANALOG_INIT_SCAN_COUNT;
        ADC_Sums[i]=0.0;
    }

    RGB_Colors[0].g=0;
    RGB_Colors[1].b=0;
    RGB_Colors[2].r=0;
    RGB_Colors[3].r=0;
    RGB_Colors[3].g=0;
    RGB_Set(RGB_Colors[0].r,RGB_Colors[0].g,RGB_Colors[0].b,0);
    RGB_Set(RGB_Colors[1].r,RGB_Colors[1].g,RGB_Colors[1].b,1);
    RGB_Set(RGB_Colors[2].r,RGB_Colors[2].g,RGB_Colors[2].b,2);
    RGB_Set(RGB_Colors[3].r,RGB_Colors[3].g,RGB_Colors[3].b,3);
    for (uint8_t i = 0; i < ADVANCED_KEY_NUM; i++)
    {
        lefl_advanced_key_set_range(Keyboard_AdvancedKeys+i,Keyboard_AdvancedKeys[i].upper_bound,Keyboard_AdvancedKeys[i].lower_bound);
    }

    for (uint8_t i = 0; i < ADVANCED_KEY_NUM; i++)
    {
        lefl_advanced_key_set_deadzone(Keyboard_AdvancedKeys+i,Keyboard_AdvancedKeys[i].upper_deadzone,Keyboard_AdvancedKeys[i].lower_deadzone);
    }
    Analog_Save();

    Communication_Add8(USART1, PROTOCOL_CMD,CMD_CALIBRATION_END);
    Communication_USART1_Transmit();
}

void Analog_Scan()
{
    for (uint8_t i = 0; i < ADVANCED_KEY_NUM; i++)
    {
        HAL_ADC_Start(&hadc1);
        HAL_ADC_PollForConversion(&hadc1,1);
        ADC_Values[i][ADC_Conversion_Count]=HAL_ADC_GetValue(&hadc1);
    }
    ADC_Conversion_Count++;
}

void Analog_Average()
{
    for (uint8_t i = 0; i < ADVANCED_KEY_NUM; i++)
    {
        ADC_Sums[i]=0;
        for (uint8_t j = 1; j < ADC_Conversion_Count; j++)
        {
            ADC_Sums[i]+=ADC_Values[i][j];
        }
        ADC_Value_List[i]=ADC_Sums[i]/(ADC_Conversion_Count-1);
    }
}

void Analog_Check()
{
    for (uint8_t i = 0; i < ADVANCED_KEY_NUM; i++)
    {
        if(Keyboard_AdvancedKeys[i].mode!=LEFL_KEY_DIGITAL_MODE)
            lefl_advanced_key_update_raw(Keyboard_AdvancedKeys+i, ADC_Value_List[i]);
    }
}


void Analog_Recovery()
{
    eeprom_buzy = true;
    for (uint16_t i = 0; i < ADVANCED_KEY_NUM; i++)
    {
        MB85RC16_ReadWord (i*64+ADVANCED_KEY_CONFIG_ADDRESS,       &(Keyboard_AdvancedKeys[i].key.id));
        MB85RC16_ReadByte (i*64+ADVANCED_KEY_CONFIG_ADDRESS+2,     &(Keyboard_AdvancedKeys[i].mode));
        MB85RC16_ReadFloat(i*64+ADVANCED_KEY_CONFIG_ADDRESS+3,     &(Keyboard_AdvancedKeys[i].trigger_distance));
        MB85RC16_ReadFloat(i*64+ADVANCED_KEY_CONFIG_ADDRESS+3+4*1, &(Keyboard_AdvancedKeys[i].release_distance));
        MB85RC16_ReadFloat(i*64+ADVANCED_KEY_CONFIG_ADDRESS+3+4*2, &(Keyboard_AdvancedKeys[i].schmitt_parameter));
        MB85RC16_ReadFloat(i*64+ADVANCED_KEY_CONFIG_ADDRESS+3+4*3, &(Keyboard_AdvancedKeys[i].trigger_speed));
        MB85RC16_ReadFloat(i*64+ADVANCED_KEY_CONFIG_ADDRESS+3+4*4, &(Keyboard_AdvancedKeys[i].release_speed));
        MB85RC16_ReadFloat(i*64+ADVANCED_KEY_CONFIG_ADDRESS+3+4*5, &(Keyboard_AdvancedKeys[i].upper_deadzone));
        MB85RC16_ReadFloat(i*64+ADVANCED_KEY_CONFIG_ADDRESS+3+4*6, &(Keyboard_AdvancedKeys[i].lower_deadzone));
        MB85RC16_ReadFloat(i*64+ADVANCED_KEY_CONFIG_ADDRESS+3+4*7, &(Keyboard_AdvancedKeys[i].range));
        MB85RC16_ReadFloat(i*64+ADVANCED_KEY_CONFIG_ADDRESS+3+4*8, &(Keyboard_AdvancedKeys[i].upper_bound));
        MB85RC16_ReadFloat(i*64+ADVANCED_KEY_CONFIG_ADDRESS+3+4*9, &(Keyboard_AdvancedKeys[i].lower_bound));
        lefl_advanced_key_set_deadzone(&Keyboard_AdvancedKeys[i], Keyboard_AdvancedKeys[i].upper_deadzone, Keyboard_AdvancedKeys[i].lower_deadzone);
    }
    eeprom_buzy = false;
}

void Analog_Save()
{
    eeprom_buzy = true;
    for (uint16_t i = 0; i < ADVANCED_KEY_NUM; i++)
    {
        MB85RC16_WriteWord (i*64+ADVANCED_KEY_CONFIG_ADDRESS,       Keyboard_AdvancedKeys[i].key.id);
        MB85RC16_WriteByte (i*64+ADVANCED_KEY_CONFIG_ADDRESS+2,     Keyboard_AdvancedKeys[i].mode);
        MB85RC16_WriteFloat(i*64+ADVANCED_KEY_CONFIG_ADDRESS+3,     Keyboard_AdvancedKeys[i].trigger_distance);
        MB85RC16_WriteFloat(i*64+ADVANCED_KEY_CONFIG_ADDRESS+3+4*1, Keyboard_AdvancedKeys[i].release_distance);
        MB85RC16_WriteFloat(i*64+ADVANCED_KEY_CONFIG_ADDRESS+3+4*2, Keyboard_AdvancedKeys[i].schmitt_parameter);
        MB85RC16_WriteFloat(i*64+ADVANCED_KEY_CONFIG_ADDRESS+3+4*3, Keyboard_AdvancedKeys[i].trigger_speed);
        MB85RC16_WriteFloat(i*64+ADVANCED_KEY_CONFIG_ADDRESS+3+4*4, Keyboard_AdvancedKeys[i].release_speed);
        MB85RC16_WriteFloat(i*64+ADVANCED_KEY_CONFIG_ADDRESS+3+4*5, Keyboard_AdvancedKeys[i].upper_deadzone);
        MB85RC16_WriteFloat(i*64+ADVANCED_KEY_CONFIG_ADDRESS+3+4*6, Keyboard_AdvancedKeys[i].lower_deadzone);
        MB85RC16_WriteFloat(i*64+ADVANCED_KEY_CONFIG_ADDRESS+3+4*7, Keyboard_AdvancedKeys[i].range);
        MB85RC16_WriteFloat(i*64+ADVANCED_KEY_CONFIG_ADDRESS+3+4*8, Keyboard_AdvancedKeys[i].upper_bound);
        MB85RC16_WriteFloat(i*64+ADVANCED_KEY_CONFIG_ADDRESS+3+4*9, Keyboard_AdvancedKeys[i].lower_bound);
    }
    eeprom_buzy = false;
}


void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
    for (uint8_t i = 0; i < ADVANCED_KEY_NUM; i++)
    {
        ADC_Values[i][ADC_Conversion_Count]=ADC_Buffer[i];
    }
    ADC_Conversion_Count++;
}
