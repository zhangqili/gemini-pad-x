/*
 * communication.c
 *
 *  Created on: Jun 11, 2023
 *      Author: xq123
 */
#include "main.h"
#include "communication.h"
#include "keyboard.h"
#include "keyboard_tree.h"
#include "analog.h"
#include "string.h"
#include "lefl.h"
#include "rgb.h"
#include "keyboard.h"

COM_CREATE(USART1)

uint16_t USART1_TX_Count=0;
uint16_t USART1_RX_Count=0;
extern bool sendreport;
extern bool sendreport_ready;
extern uint8_t cmd_buffer;
extern uint8_t spi_flag;
extern uint8_t spi_rx[8];

extern DMA_HandleTypeDef hdma_usart1_rx;

void Communication_Pack_Advanced_Key(uint8_t index,lefl_advanced_key_t *k)
{
    Communication_Add8(USART1,PROTOCOL_ADVANCED_KEY_SET,index);
    Communication_Add8(USART1,PROTOCOL_ADVANCED_KEY_MODE,k->mode);
    Communication_Add32(USART1,PROTOCOL_ADVANCED_KEY_TRIGGER_DISTANCE,k->trigger_distance);
    Communication_Add32(USART1,PROTOCOL_ADVANCED_KEY_RELEASE_DISTANCE,k->release_distance);
    Communication_Add32(USART1,PROTOCOL_ADVANCED_KEY_SCHMITT_PARAMETER,k->schmitt_parameter);
    Communication_Add32(USART1,PROTOCOL_ADVANCED_KEY_TRIGGER_SPEED,k->trigger_speed);
    Communication_Add32(USART1,PROTOCOL_ADVANCED_KEY_RELEASE_SPEED,k->release_speed);
    Communication_Add32(USART1,PROTOCOL_ADVANCED_KEY_UPPER_DEADZONE,k->upper_deadzone);
    Communication_Add32(USART1,PROTOCOL_ADVANCED_KEY_LOWER_DEADZONE,k->lower_deadzone);
    Communication_Add32(USART1,PROTOCOL_ADVANCED_KEY_UPPER_BOUND,k->upper_bound);
    Communication_Add32(USART1,PROTOCOL_ADVANCED_KEY_LOWER_BOUND,k->lower_bound);
    if(USART1_TX_Length<64)USART1_TX_Length+=(2*2+5*9);
}

void Communication_Unpack_Advanced_Key(uint8_t i)
{
    uint8_t index = USART1_RX_Buffer[i+1];
    i+=2;
    Keyboard_AdvancedKeys[index].mode = USART1_RX_Buffer[i+1];
    i+=2;
    memcpy((uint8_t*)&(Keyboard_AdvancedKeys[index].trigger_distance),USART1_RX_Buffer+i+1,4);
    i+=5;
    memcpy((uint8_t*)&(Keyboard_AdvancedKeys[index].release_distance),USART1_RX_Buffer+i+1,4);
    i+=5;
    memcpy((uint8_t*)&(Keyboard_AdvancedKeys[index].schmitt_parameter),USART1_RX_Buffer+i+1,4);
    i+=5;
    memcpy((uint8_t*)&(Keyboard_AdvancedKeys[index].trigger_speed),USART1_RX_Buffer+i+1,4);
    i+=5;
    memcpy((uint8_t*)&(Keyboard_AdvancedKeys[index].release_speed),USART1_RX_Buffer+i+1,4);
    i+=5;
    memcpy((uint8_t*)&(Keyboard_AdvancedKeys[index].upper_deadzone),USART1_RX_Buffer+i+1,4);
    i+=5;
    memcpy((uint8_t*)&(Keyboard_AdvancedKeys[index].lower_deadzone),USART1_RX_Buffer+i+1,4);
    i+=5;
    memcpy((uint8_t*)&(Keyboard_AdvancedKeys[index].upper_bound),USART1_RX_Buffer+i+1,4);
    i+=5;
    memcpy((uint8_t*)&(Keyboard_AdvancedKeys[index].lower_bound),USART1_RX_Buffer+i+1,4);
    i+=5;
    lefl_advanced_key_set_range(Keyboard_AdvancedKeys+index, Keyboard_AdvancedKeys[index].upper_bound, Keyboard_AdvancedKeys[index].lower_bound);
    lefl_advanced_key_set_deadzone(Keyboard_AdvancedKeys+index, Keyboard_AdvancedKeys[index].upper_deadzone, Keyboard_AdvancedKeys[index].lower_deadzone);
}

void Communication_Pack_Key(uint8_t index,uint8_t function_key,uint16_t id)
{
    USART1_TX_Buffer[USART1_TX_Length]=(PROTOCOL_TARGET_KEY_AND_BINDING);
    USART1_TX_Buffer[USART1_TX_Length+1]=index;
    USART1_TX_Buffer[USART1_TX_Length+2]=index;
    memcpy(USART1_TX_Buffer+3,(uint8_t*)&id,2);
    if(USART1_TX_Length<64)USART1_TX_Length+=5;
}

void Communication_Unpack_Key(uint8_t i)
{
    uint8_t index = USART1_RX_Buffer[i+1];
    uint8_t function_key = USART1_RX_Buffer[i+2];
    uint16_t id;
    memcpy((uint8_t*)&id,&USART1_RX_Buffer[i+3],2);
    switch (function_key)
    {
        case 0:
            if(index<ADVANCED_KEY_NUM)
            {
                Keyboard_AdvancedKeys[index].key.id=id;
            }
            else
            {
                Keyboard_Keys[index-ADVANCED_KEY_NUM].id=id;
            }
            break;
        case 1:
            if(index<ADVANCED_KEY_NUM)
            {
                Keyboard_Advanced_SHIFT_IDs[index]=id;
            }
            else
            {
                Keyboard_SHIFT_IDs[index-ADVANCED_KEY_NUM]=id;
            }
            break;
        case 2:
            if(index<ADVANCED_KEY_NUM)
            {
                Keyboard_Advanced_ALPHA_IDs[index]=id;
            }
            else
            {
                Keyboard_ALPHA_IDs[index-ADVANCED_KEY_NUM]=id;
            }
            break;
        default:
            break;
    }
}

void Communication_Pack()
{
    USART1_TX_Length=0;
    //Communication_Add(1,2);
    Communication_Add8(USART1, PROTOCOL_KEYS0,
            Keyboard_AdvancedKeys[0].key.state<<3|
            Keyboard_AdvancedKeys[1].key.state<<2|
            Keyboard_AdvancedKeys[2].key.state<<1|
            Keyboard_AdvancedKeys[3].key.state);

    //Communication_Add(PROTOCOL_KEYS0,Keyboard_Keys[0]<<3|Keyboard_Keys[1]<<2|Keyboard_Keys[2]<<1|Keyboard_Keys[3]|Keyboard_Keys[4]<<7|Keyboard_Keys[5]<<6|Keyboard_Keys[6]<<5|Keyboard_Keys[8]<<4);
    Communication_Add8(USART1, PROTOCOL_KEYS1,(
            KEY_SHIFT.state<<7|
            KEY_ALPHA.state<<6|
            KEY_KNOB.state<<5|
            KEY_WHEEL.state<<4|
            KEY_KNOB_CLOCKWISE.state<<3|
            KEY_KNOB_ANTICLOCKWISE.state<<2|
            KEY_WHEEL_CLOCKWISE.state<<1|
            KEY_WHEEL_ANTICLOCKWISE.state<<0));
    //Communication_Add(PROTOCOL_KEYS1,(Keyboard_EC11Flag?(Keyboard_Keys[7]?BIT(3):BIT(2)):0));

    //Communication_Add(PROTOCOL_ANALOG1,(uint8_t)(analog_keys[0].upper_bound/16.0));
    //Communication_Add(PROTOCOL_ANALOG2,(uint8_t)(analog_keys[0].lower_bound/16.0));
    //Communication_Add(PROTOCOL_ANALOG3,(uint8_t)(Analog_Values[2]*256.0));
    //Communication_Add(PROTOCOL_ANALOG4,(uint8_t)(Analog_Values[3]*256.0));

    /*
    Communication_Add8(PROTOCOL_ANALOG1,(uint8_t)(ADC_Value_List[0]/16));
    Communication_Add8(PROTOCOL_ANALOG2,(uint8_t)(ADC_Value_List[1]/16));
    Communication_Add8(PROTOCOL_ANALOG3,(uint8_t)(ADC_Value_List[2]/16));
    Communication_Add8(PROTOCOL_ANALOG4,(uint8_t)(ADC_Value_List[3]/16));
    */
    /*
    Communication_Add8(PROTOCOL_ANALOG1,(uint8_t)(advanced_keys[0].value<0?0:(advanced_keys[0].value>1.0?255:advanced_keys[0].value*255)));
    Communication_Add8(PROTOCOL_ANALOG2,(uint8_t)(advanced_keys[1].value<0?0:(advanced_keys[1].value>1.0?255:advanced_keys[1].value*255)));
    Communication_Add8(PROTOCOL_ANALOG3,(uint8_t)(advanced_keys[2].value<0?0:(advanced_keys[2].value>1.0?255:advanced_keys[2].value*255)));
    Communication_Add8(PROTOCOL_ANALOG4,(uint8_t)(advanced_keys[3].value<0?0:(advanced_keys[3].value>1.0?255:advanced_keys[3].value*255)));
    */

    Communication_Add32(USART1, PROTOCOL_ANALOG1_RAW,Keyboard_AdvancedKeys[0].raw);
    Communication_Add32(USART1, PROTOCOL_ANALOG2_RAW,Keyboard_AdvancedKeys[1].raw);
    Communication_Add32(USART1, PROTOCOL_ANALOG3_RAW,Keyboard_AdvancedKeys[2].raw);
    Communication_Add32(USART1, PROTOCOL_ANALOG4_RAW,Keyboard_AdvancedKeys[3].raw);
    Communication_Add8(USART1, PROTOCOL_SCAN_COUNT, ADC_Conversion_Count);
    Communication_Add8(USART1, PROTOCOL_DEBUG,(uint8_t)(Keyboard_Tree_BaseStatus));
    extern uint8_t Slave_SPI_RxBuffer[16];
    extern uint8_t count;
    //Communication_Add8(USART1, PROTOCOL_DEBUG,(uint8_t)(Keyboard_TNodes[0].Status));
    //Communication_Add8(USART1, PROTOCOL_DEBUG,(uint8_t)(count));
    //Communication_Add(3,4);
}


void Communication_Unpack(UART_HandleTypeDef *huart)
{
    if(RESET != __HAL_UART_GET_FLAG(huart, UART_FLAG_IDLE))
    {
        __HAL_UART_CLEAR_IDLEFLAG(huart);
        HAL_UART_DMAStop(huart);

        if(huart->Instance==USART1)
        {
            USART1_RX_Length = BUFFER_LENGTH - __HAL_DMA_GET_COUNTER(&hdma_usart1_rx);
            //if(USART1_RX_Buffer[0]==1&&USART1_RX_Buffer[1]==2&&USART1_RX_Buffer[USART1_RX_Length-2]==3&&USART1_RX_Buffer[USART1_RX_Length-1]==4)
            if(USART1_RX_Buffer[USART1_RX_Length-1]==USART1_RX_Length)
            {
                for(uint8_t i = 0;i<USART1_RX_Length-1;)
                {
                    switch(USART1_RX_Buffer[i])
                    {
                        case PROTOCOL_CMD:
                            switch (USART1_RX_Buffer[i+1])
                            {
                                case CMD_RESET:
                                    __set_FAULTMASK(1);
                                    HAL_NVIC_SystemReset();
                                    break;
                                default:
                                    cmd_buffer=USART1_RX_Buffer[i+1];
                                    break;
                            }
                            i+=2;
                            break;
                        case PROTOCOL_SET_TARGET_RGB:
                            RGB_TargetConfig=USART1_RX_Buffer[i+1];
                            i+=2;
                            break;
                        case PROTOCOL_RGB_MODE:
                            if(RGB_TargetConfig)
                            {
                                RGB_Configs[RGB_TargetConfig-1].mode=USART1_RX_Buffer[i+1];
                            }
                            else
                            {
                                RGB_GlobalConfig.mode=USART1_RX_Buffer[i+1];
                            }
                            i+=2;
                            break;
                        case PROTOCOL_COLOR_RGB_R:
                            if(RGB_TargetConfig)
                            {
                                RGB_Configs[RGB_TargetConfig-1].rgb.r=USART1_RX_Buffer[i+1];
                            }
                            else
                            {
                                RGB_GlobalConfig.rgb.r=USART1_RX_Buffer[i+1];
                            }
                            i+=2;
                            break;
                        case PROTOCOL_COLOR_RGB_G:
                            if(RGB_TargetConfig)
                            {
                                RGB_Configs[RGB_TargetConfig-1].rgb.g=USART1_RX_Buffer[i+1];
                            }
                            else
                            {
                                RGB_GlobalConfig.rgb.g=USART1_RX_Buffer[i+1];
                            }
                            i+=2;
                            break;
                        case PROTOCOL_COLOR_RGB_B:
                            if(RGB_TargetConfig)
                            {
                                RGB_Configs[RGB_TargetConfig-1].rgb.b=USART1_RX_Buffer[i+1];
                            }
                            else
                            {
                                RGB_GlobalConfig.rgb.b=USART1_RX_Buffer[i+1];
                            }
                            i+=2;
                            break;
                        case PROTOCOL_COLOR_HSV_S:
                            if(RGB_TargetConfig)
                            {
                                RGB_Configs[RGB_TargetConfig-1].hsv.s=USART1_RX_Buffer[i+1];
                            }
                            else
                            {
                                RGB_GlobalConfig.hsv.s=USART1_RX_Buffer[i+1];
                            }
                            i+=2;
                            break;
                        case PROTOCOL_COLOR_HSV_V:
                            if(RGB_TargetConfig)
                            {
                                RGB_Configs[RGB_TargetConfig-1].hsv.v=USART1_RX_Buffer[i+1];
                            }
                            else
                            {
                                RGB_GlobalConfig.hsv.v=USART1_RX_Buffer[i+1];
                            }
                            i+=2;
                            break;
                        case PROTOCOL_RGB_SPEED:
                            if(RGB_TargetConfig)
                            {
                                memcpy((uint8_t *)&(RGB_Configs[RGB_TargetConfig-1].speed),USART1_RX_Buffer+i+1,4);
                            }
                            else
                            {
                                memcpy((uint8_t *)&(RGB_GlobalConfig.speed),USART1_RX_Buffer+i+1,4);
                            }
                            i+=5;
                            break;
                        case PROTOCOL_TARGET_KEY_AND_BINDING:
                            Communication_Unpack_Key(i);
                            i+=5;
                            break;
                        case PROTOCOL_ADVANCED_KEY_SET:
                            Keyboard_TargetAdvancedKey = USART1_RX_Buffer[i+1];
                            i+=2;
                        case PROTOCOL_ADVANCED_KEY_MODE:
                            Keyboard_AdvancedKeys[Keyboard_TargetAdvancedKey].mode = USART1_RX_Buffer[i+1];
                            i+=2;
                            break;
                        case PROTOCOL_ADVANCED_KEY_TRIGGER_DISTANCE:
                            memcpy((uint8_t*)&(Keyboard_AdvancedKeys[Keyboard_TargetAdvancedKey].trigger_distance),USART1_RX_Buffer+i+1,4);
                            i+=5;
                            break;
                        case PROTOCOL_ADVANCED_KEY_RELEASE_DISTANCE:
                            memcpy((uint8_t*)&(Keyboard_AdvancedKeys[Keyboard_TargetAdvancedKey].release_distance),USART1_RX_Buffer+i+1,4);
                            i+=5;
                            break;
                        case PROTOCOL_ADVANCED_KEY_SCHMITT_PARAMETER:
                            memcpy((uint8_t*)&(Keyboard_AdvancedKeys[Keyboard_TargetAdvancedKey].schmitt_parameter),USART1_RX_Buffer+i+1,4);
                            i+=5;
                            break;
                        case PROTOCOL_ADVANCED_KEY_TRIGGER_SPEED:
                            memcpy((uint8_t*)&(Keyboard_AdvancedKeys[Keyboard_TargetAdvancedKey].trigger_speed),USART1_RX_Buffer+i+1,4);
                            i+=5;
                            break;
                        case PROTOCOL_ADVANCED_KEY_RELEASE_SPEED:
                            memcpy((uint8_t*)&(Keyboard_AdvancedKeys[Keyboard_TargetAdvancedKey].release_speed),USART1_RX_Buffer+i+1,4);
                            i+=5;
                            break;
                        case PROTOCOL_ADVANCED_KEY_UPPER_DEADZONE:
                            memcpy((uint8_t*)&(Keyboard_AdvancedKeys[Keyboard_TargetAdvancedKey].upper_deadzone),USART1_RX_Buffer+i+1,4);
                            i+=5;
                            break;
                        case PROTOCOL_ADVANCED_KEY_LOWER_DEADZONE:
                            memcpy((uint8_t*)&(Keyboard_AdvancedKeys[Keyboard_TargetAdvancedKey].lower_deadzone),USART1_RX_Buffer+i+1,4);
                            i+=5;
                            break;
                        case PROTOCOL_ADVANCED_KEY_UPPER_BOUND:
                            memcpy((uint8_t*)&(Keyboard_AdvancedKeys[Keyboard_TargetAdvancedKey].upper_bound),USART1_RX_Buffer+i+1,4);
                            i+=5;
                            break;
                        case PROTOCOL_ADVANCED_KEY_LOWER_BOUND:
                            memcpy((uint8_t*)&(Keyboard_AdvancedKeys[Keyboard_TargetAdvancedKey].lower_bound),USART1_RX_Buffer+i+1,4);
                            i+=5;
                            break;
                        case PROTOCOL_ADVANCED_KEY_REFRESH:
                            lefl_advanced_key_set_range(Keyboard_AdvancedKeys+Keyboard_TargetAdvancedKey, Keyboard_AdvancedKeys[Keyboard_TargetAdvancedKey].upper_bound, Keyboard_AdvancedKeys[Keyboard_TargetAdvancedKey].lower_bound);
                            lefl_advanced_key_set_deadzone(Keyboard_AdvancedKeys+Keyboard_TargetAdvancedKey, Keyboard_AdvancedKeys[Keyboard_TargetAdvancedKey].upper_deadzone, Keyboard_AdvancedKeys[Keyboard_TargetAdvancedKey].lower_deadzone);
                            i+=2;
                        default:
                            i+=2;
                            break;
                    }
                }
            }
            HAL_UART_Receive_DMA(huart, USART1_RX_Buffer, BUFFER_LENGTH);
        }
    }
}


