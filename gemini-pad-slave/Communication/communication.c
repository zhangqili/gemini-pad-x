/*
 * communication.c
 *
 *  Created on: Jun 11, 2023
 *      Author: xq123
 */
#include "main.h"
#include "communication.h"
#include "lefl.h"
#include "fezui.h"
#include "fezui_var.h"

COM_CREATE(USART1)

uint32_t USART1_TX_Count=0;
uint32_t USART1_RX_Count=0;

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
                USART1_RX_Count++;
                for(uint8_t i = 0;i<USART1_RX_Length-1;)
                {
                    switch(USART1_RX_Buffer[i])
                    {
                    case PROTOCOL_KEYS0:
                        /*
                        analog_keys[0].state=(USART1_RX_Buffer[i+1]&BIT(3));
                        analog_keys[1].state=(USART1_RX_Buffer[i+1]&BIT(2));
                        analog_keys[2].state=(USART1_RX_Buffer[i+1]&BIT(1));
                        analog_keys[3].state=(USART1_RX_Buffer[i+1]&BIT(0));
                        */
                        //lefl_analog_key_update(analog_keys+0, (USART1_RX_Buffer[i+1]&BIT(3)));
                        //lefl_analog_key_update(analog_keys+2, (USART1_RX_Buffer[i+1]&BIT(2)));
                        //lefl_analog_key_update(analog_keys+3, (USART1_RX_Buffer[i+1]&BIT(1)));
                        //lefl_analog_key_update(analog_keys+4, (USART1_RX_Buffer[i+1]&BIT(0)));
                        key_buffer[0]=(USART1_RX_Buffer[i+1]&BIT(3));
                        key_buffer[1]=(USART1_RX_Buffer[i+1]&BIT(2));
                        key_buffer[2]=(USART1_RX_Buffer[i+1]&BIT(1));
                        key_buffer[3]=(USART1_RX_Buffer[i+1]&BIT(0));
                        i+=2;
                        break;
                    case PROTOCOL_KEYS1:
                        /*
                        key_buffer[4]=(USART1_RX_Buffer[i+1]&BIT(7));
                        key_buffer[5]=(USART1_RX_Buffer[i+1]&BIT(6));
                        key_buffer[6]=(USART1_RX_Buffer[i+1]&BIT(5));
                        key_buffer[7]=(USART1_RX_Buffer[i+1]&BIT(4));
                        key_buffer[8]=(USART1_RX_Buffer[i+1]&BIT(3));
                        key_buffer[9]=(USART1_RX_Buffer[i+1]&BIT(2));
                        key_buffer[10]=(USART1_RX_Buffer[i+1]&BIT(1));
                        key_buffer[11]=(USART1_RX_Buffer[i+1]&BIT(0));
                        */
                        lefl_key_update(Keyboard_Keys+0, (USART1_RX_Buffer[i+1]&BIT(7)));
                        lefl_key_update(Keyboard_Keys+1, (USART1_RX_Buffer[i+1]&BIT(6)));
                        lefl_key_update(Keyboard_Keys+2, (USART1_RX_Buffer[i+1]&BIT(5)));
                        lefl_key_update(Keyboard_Keys+3, (USART1_RX_Buffer[i+1]&BIT(4)));
                        lefl_key_update(Keyboard_Keys+4, (USART1_RX_Buffer[i+1]&BIT(3)));
                        lefl_key_update(Keyboard_Keys+5, (USART1_RX_Buffer[i+1]&BIT(2)));
                        lefl_key_update(Keyboard_Keys+6, (USART1_RX_Buffer[i+1]&BIT(1)));
                        lefl_key_update(Keyboard_Keys+7, (USART1_RX_Buffer[i+1]&BIT(0)));
                        i+=2;
                        break;
                    case PROTOCOL_ANALOG1_RAW:
                        memcpy((uint8_t *)&(Keyboard_AdvancedKeys[0].raw),USART1_RX_Buffer+i+1,4);
                        lefl_advanced_key_update_raw(Keyboard_AdvancedKeys + 0, (uint16_t)Keyboard_AdvancedKeys[0].raw);
                        i+=5;
                        break;
                    case PROTOCOL_ANALOG2_RAW:
                        memcpy((uint8_t *)&(Keyboard_AdvancedKeys[1].raw),USART1_RX_Buffer+i+1,4);
                        lefl_advanced_key_update_raw(Keyboard_AdvancedKeys + 1, (uint16_t)Keyboard_AdvancedKeys[1].raw);
                        i+=5;
                        break;
                    case PROTOCOL_ANALOG3_RAW:
                        memcpy((uint8_t *)&(Keyboard_AdvancedKeys[2].raw),USART1_RX_Buffer+i+1,4);
                        lefl_advanced_key_update_raw(Keyboard_AdvancedKeys + 2, (uint16_t)Keyboard_AdvancedKeys[2].raw);
                        i+=5;
                        break;
                    case PROTOCOL_ANALOG4_RAW:
                        memcpy((uint8_t *)&(Keyboard_AdvancedKeys[3].raw),USART1_RX_Buffer+i+1,4);
                        lefl_advanced_key_update_raw(Keyboard_AdvancedKeys + 3, (uint16_t)Keyboard_AdvancedKeys[3].raw);
                        i+=5;
                        break;
                    case PROTOCOL_CMD:
                        switch (USART1_RX_Buffer[i+1])
                        {
                            case CMD_EEPROM_ENABLE:
                            case CMD_EEPROM_DISABLE:
                                eeprom_enable = USART1_RX_Buffer[i+1]-CMD_EEPROM_DISABLE;
                                break;
                            default:
                                cmd_buffer=USART1_RX_Buffer[i+1];
                                break;
                        }
                        i+=2;
                        break;
                    case PROTOCOL_DEBUG:
                        fezui_debug=USART1_RX_Buffer[i+1];
                        i+=2;
                        break;
                    case PROTOCOL_DEBUG_FLOAT:
                        memcpy((uint8_t *)&(fezui_debug_float),USART1_RX_Buffer+i+1,4);
                        i+=5;
                        break;
                    case PROTOCOL_SCAN_COUNT:
                        fezui_adc_conversion_count = USART1_RX_Buffer[i+1];
                        i+=2;
                        break;
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




