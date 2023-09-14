/*
 * rgb.c
 *
 *  Created on: May 21, 2023
 *      Author: xq123
 */
#include "main.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "rgb.h"
#include "MB85RC16.h"

#ifdef USE_RGB
uint32_t RGB_Buffer[RGB_BUFFER_LENGTH];
#endif
rgb_global_config_t RGB_GlobalConfig;
rgb_individual_config_t RGB_Configs[RGB_NUM];
lefl_color_rgb_t RGB_Colors[RGB_NUM];
uint8_t RGB_TargetConfig;

#ifdef USE_RGB
void RGB_Init()
{
    for (uint16_t i = 0; i < 400; i++) {
        RGB_Buffer[i] = NONE_PULSE;
    }
}

#define COLOR_INTERVAL(key,low,up) (uint8_t)((key)<0?(low):((key)>1.0?(up):(key)*(up)))
void RGB_Update()
{
    lefl_color_hsv_t temp_hsv={.h=0,.s=240,.v=120};
    switch (RGB_GlobalConfig.mode)
    {
        case RGB_GLOBAL_MODE_INDIVIDUAL:
            for (uint16_t i = 0; i < RGB_NUM; i++)
            {
                switch (RGB_Configs[i].mode)
                {
                    case RGB_MODE_REACT_LINEAR:
                        RGB_Colors[i].r=COLOR_INTERVAL(Keyboard_AdvancedKeys[i].value,0,(float)(RGB_Configs[i].rgb.r));
                        RGB_Colors[i].g=COLOR_INTERVAL(Keyboard_AdvancedKeys[i].value,0,(float)(RGB_Configs[i].rgb.g));
                        RGB_Colors[i].b=COLOR_INTERVAL(Keyboard_AdvancedKeys[i].value,0,(float)(RGB_Configs[i].rgb.b));
                        break;
                    case RGB_MODE_REACT_TRIGGER:
                        RGB_Colors[i].r=Keyboard_AdvancedKeys[i].key.state?(RGB_Configs[i].rgb.r):(RGB_Colors[i].r?RGB_Colors[i].r-1:0);
                        RGB_Colors[i].g=Keyboard_AdvancedKeys[i].key.state?(RGB_Configs[i].rgb.g):(RGB_Colors[i].r?RGB_Colors[i].r-1:0);
                        RGB_Colors[i].b=Keyboard_AdvancedKeys[i].key.state?(RGB_Configs[i].rgb.b):(RGB_Colors[i].r?RGB_Colors[i].r-1:0);
                        break;
                    default:
                        break;
                }
                RGB_Buffer[i] = NONE_PULSE;
            }
            break;
        case RGB_GLOBAL_MODE_WAVE:
            RGB_GlobalConfig.argument+=RGB_GlobalConfig.speed;
            if(RGB_GlobalConfig.argument>359.9f)
            {
                RGB_GlobalConfig.argument=0;
            }
            if(RGB_GlobalConfig.argument<0)
            {
                RGB_GlobalConfig.argument=360;
            }
            for (uint16_t i = 0; i < RGB_NUM; i++)
            {
                temp_hsv.h = ((uint16_t)RGB_GlobalConfig.argument + i*30)%360;
                lefl_color_set_hsv(RGB_Colors+i, &temp_hsv);
            }
            break;
        default:
            break;
    }
    RGB_Set(RGB_Colors[0].r,RGB_Colors[0].g,RGB_Colors[0].b,0);
    RGB_Set(RGB_Colors[1].r,RGB_Colors[1].g,RGB_Colors[1].b,1);
    RGB_Set(RGB_Colors[2].r,RGB_Colors[2].g,RGB_Colors[2].b,2);
    RGB_Set(RGB_Colors[3].r,RGB_Colors[3].g,RGB_Colors[3].b,3);
}

void RGB_Set(uint8_t r,uint8_t g,uint8_t b,uint16_t index)
{
  for (uint8_t i = 0;i < 8;i++)
  {
    RGB_Buffer[RGB_RESET_LENGTH+index*24+i]      = (g << i) & (0x80)?ONE_PULSE:ZERO_PULSE;
    RGB_Buffer[RGB_RESET_LENGTH+index*24+i + 8]  = (r << i) & (0x80)?ONE_PULSE:ZERO_PULSE;
    RGB_Buffer[RGB_RESET_LENGTH+index*24+i + 16] = (b << i) & (0x80)?ONE_PULSE:ZERO_PULSE;
  }
}

void RGB_Flash()
{
    for (uint8_t i=1;i<127;i++)
    {
      RGB_Set(i,i,i,0);
      RGB_Set(i,i,i,1);
      RGB_Set(i,i,i,2);
      RGB_Set(i,i,i,3);
      HAL_Delay(2);
    }
    for (uint8_t i=128;i>3;i--)
    {
      RGB_Set(i,i,i,0);
      RGB_Set(i,i,i,1);
      RGB_Set(i,i,i,2);
      RGB_Set(i,i,i,3);
      HAL_Delay(2);
    }
    RGB_TurnOff();
}

void RGB_TurnOff()
{
	for (uint8_t i=0;i<RGB_NUM;i++)
    {
        RGB_Set(0,0,0,i);
    }
}
#endif

void RGB_Recovery()
{

    if(eeprom_enable)
    {

        MB85RC16_ReadByte (RGB_GLOBAL_CONFIG_ADDRESS+0, &RGB_GlobalConfig.mode);
        MB85RC16_ReadByte (RGB_GLOBAL_CONFIG_ADDRESS+1, &RGB_GlobalConfig.rgb.r);
        MB85RC16_ReadByte (RGB_GLOBAL_CONFIG_ADDRESS+2, &RGB_GlobalConfig.rgb.g);
        MB85RC16_ReadByte (RGB_GLOBAL_CONFIG_ADDRESS+3, &RGB_GlobalConfig.rgb.b);
        MB85RC16_ReadWord (RGB_GLOBAL_CONFIG_ADDRESS+4, &RGB_GlobalConfig.hsv.h);
        MB85RC16_ReadByte (RGB_GLOBAL_CONFIG_ADDRESS+6, &RGB_GlobalConfig.hsv.s);
        MB85RC16_ReadByte (RGB_GLOBAL_CONFIG_ADDRESS+7, &RGB_GlobalConfig.hsv.v);
        MB85RC16_ReadFloat(RGB_GLOBAL_CONFIG_ADDRESS+8, &RGB_GlobalConfig.speed);
        for (uint16_t i = 0; i < ADVANCED_KEY_NUM; i++)
        {
            MB85RC16_ReadByte (RGB_CONFIG_ADDRESS+i*16+0, &RGB_Configs[i].mode);
            MB85RC16_ReadByte (RGB_CONFIG_ADDRESS+i*16+1, &RGB_Configs[i].rgb.r);
            MB85RC16_ReadByte (RGB_CONFIG_ADDRESS+i*16+2, &RGB_Configs[i].rgb.g);
            MB85RC16_ReadByte (RGB_CONFIG_ADDRESS+i*16+3, &RGB_Configs[i].rgb.b);
            MB85RC16_ReadWord (RGB_CONFIG_ADDRESS+i*16+4, &RGB_Configs[i].hsv.h);
            MB85RC16_ReadByte (RGB_CONFIG_ADDRESS+i*16+6, &RGB_Configs[i].hsv.s);
            MB85RC16_ReadByte (RGB_CONFIG_ADDRESS+i*16+7, &RGB_Configs[i].hsv.v);
            MB85RC16_ReadFloat(RGB_CONFIG_ADDRESS+i*16+8, &RGB_Configs[i].speed);
        }
    }
}

void RGB_Save()
{
    if(eeprom_enable)
    {
        MB85RC16_WriteByte (RGB_GLOBAL_CONFIG_ADDRESS+0, RGB_GlobalConfig.mode);
        MB85RC16_WriteByte (RGB_GLOBAL_CONFIG_ADDRESS+1, RGB_GlobalConfig.rgb.r);
        MB85RC16_WriteByte (RGB_GLOBAL_CONFIG_ADDRESS+2, RGB_GlobalConfig.rgb.g);
        MB85RC16_WriteByte (RGB_GLOBAL_CONFIG_ADDRESS+3, RGB_GlobalConfig.rgb.b);
        MB85RC16_WriteWord (RGB_GLOBAL_CONFIG_ADDRESS+4, RGB_GlobalConfig.hsv.h);
        MB85RC16_WriteByte (RGB_GLOBAL_CONFIG_ADDRESS+6, RGB_GlobalConfig.hsv.s);
        MB85RC16_WriteByte (RGB_GLOBAL_CONFIG_ADDRESS+7, RGB_GlobalConfig.hsv.v);
        MB85RC16_WriteFloat(RGB_GLOBAL_CONFIG_ADDRESS+8, RGB_GlobalConfig.speed);
        for (uint16_t i = 0; i < ADVANCED_KEY_NUM; i++)
        {
            MB85RC16_WriteByte (RGB_CONFIG_ADDRESS+i*16+0, RGB_Configs[i].mode);
            MB85RC16_WriteByte (RGB_CONFIG_ADDRESS+i*16+1, RGB_Configs[i].rgb.r);
            MB85RC16_WriteByte (RGB_CONFIG_ADDRESS+i*16+2, RGB_Configs[i].rgb.g);
            MB85RC16_WriteByte (RGB_CONFIG_ADDRESS+i*16+3, RGB_Configs[i].rgb.b);
            MB85RC16_WriteWord (RGB_CONFIG_ADDRESS+i*16+4, RGB_Configs[i].hsv.h);
            MB85RC16_WriteByte (RGB_CONFIG_ADDRESS+i*16+6, RGB_Configs[i].hsv.s);
            MB85RC16_WriteByte (RGB_CONFIG_ADDRESS+i*16+7, RGB_Configs[i].hsv.v);
            MB85RC16_WriteFloat(RGB_CONFIG_ADDRESS+i*16+8, RGB_Configs[i].speed);
        }
    }
}
