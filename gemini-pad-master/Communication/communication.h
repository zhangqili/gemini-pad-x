/*
 * communication.h
 *
 *  Created on: Jun 11, 2023
 *      Author: xq123
 */

#ifndef COMMUNICATION_H_
#define COMMUNICATION_H_
#include "stdint.h"
#include "lefl.h"

#define BUFFER_LENGTH 64


#define COM_CREATE(u)\
uint8_t u##_RX_Buffer[BUFFER_LENGTH];\
uint8_t u##_RX_Length=0;\
uint8_t u##_TX_Buffer[BUFFER_LENGTH];\
uint8_t u##_TX_Length=0;

#define COM_DECLARE(u)\
extern uint8_t u##_RX_Buffer[BUFFER_LENGTH];\
extern uint8_t u##_RX_Length;\
extern uint8_t u##_TX_Buffer[BUFFER_LENGTH];\
extern uint8_t u##_TX_Length;

#define Communication_SOH(u) u##_TX_Buffer[0]=1;
#define Communication_STX(u) u##_TX_Buffer[1]=2;
#define Communication_ETX(u) u##_TX_Buffer[USART1_TX_Length]=3;
#define Communication_EOH(u) u##_TX_Buffer[USART1_TX_Length]=4;

#define Communication_Add8(u,a,b) u##_TX_Buffer[u##_TX_Length]=(a);\
                               u##_TX_Buffer[u##_TX_Length+1]=(b);\
                               if(u##_TX_Length<BUFFER_LENGTH){u##_TX_Length+=2;}

#define Communication_Add32(u,a,b) u##_TX_Buffer[u##_TX_Length]=(a);\
                               memcpy(u##_TX_Buffer+u##_TX_Length+1,(uint8_t*)&(b),4);\
                               if(u##_TX_Length<BUFFER_LENGTH)u##_TX_Length+=5;

#define Communication_AddLength(u)  u##_TX_Buffer[u##_TX_Length]=(u##_TX_Length+1);\
                                    u##_TX_Length++;


#define Communication_Enable(huart,buf,len) HAL_UART_Receive_DMA(huart,buf,len);\
                                            __HAL_UART_ENABLE_IT(huart, UART_IT_IDLE);


#define Communication_USART1_Transmit()    	Communication_AddLength(USART1);\
                                    		HAL_UART_Transmit_DMA(&huart1,USART1_TX_Buffer,USART1_TX_Length);\
                                    		USART1_TX_Length=0;


typedef enum
{
    //0x00
    PROTOCOL_KEYS0=0x00,
    PROTOCOL_KEYS1,
    //0x10
    PROTOCOL_ANALOG1=0x10,
    PROTOCOL_ANALOG2,
    PROTOCOL_ANALOG3,
    PROTOCOL_ANALOG4,
    //0x20
    PROTOCOL_ANALOG1_RAW=0x20,
    PROTOCOL_ANALOG2_RAW,
    PROTOCOL_ANALOG3_RAW,
    PROTOCOL_ANALOG4_RAW,
    //0x30
    PROTOCOL_SCAN_COUNT=0x30,
    PROTOCOL_CMD,
    //0x40
    PROTOCOL_TARGET_KEY_AND_BINDING=0x40,
    PROTOCOL_ADVANCED_KEY_SET,
    PROTOCOL_ADVANCED_KEY_MODE,
    PROTOCOL_ADVANCED_KEY_TRIGGER_DISTANCE,
    PROTOCOL_ADVANCED_KEY_RELEASE_DISTANCE,
    PROTOCOL_ADVANCED_KEY_SCHMITT_PARAMETER,
    PROTOCOL_ADVANCED_KEY_TRIGGER_SPEED,
    PROTOCOL_ADVANCED_KEY_RELEASE_SPEED,
    PROTOCOL_ADVANCED_KEY_UPPER_DEADZONE,
    PROTOCOL_ADVANCED_KEY_LOWER_DEADZONE,
    PROTOCOL_ADVANCED_KEY_RANGE,
    PROTOCOL_ADVANCED_KEY_UPPER_BOUND,
    PROTOCOL_ADVANCED_KEY_LOWER_BOUND,
    PROTOCOL_ADVANCED_KEY_REFRESH,
    //0x50
    PROTOCOL_SET_TARGET_RGB=0x50,
    PROTOCOL_RGB_MODE,
    PROTOCOL_COLOR_RGB_R,
    PROTOCOL_COLOR_RGB_G,
    PROTOCOL_COLOR_RGB_B,
    PROTOCOL_COLOR_HSV_H_L,
    PROTOCOL_COLOR_HSV_H_H,
    PROTOCOL_COLOR_HSV_S,
    PROTOCOL_COLOR_HSV_V,
    PROTOCOL_RGB_SPEED,
    //0xF0
    PROTOCOL_DEBUG=0xF0,
    PROTOCOL_DEBUG_FLOAT=0xF1,
} USART_PROTOCOL;


typedef enum
{
    //0x00
    CMD_NULL,
    CMD_CALIBRATION_START,
    CMD_CALIBRATION_UP,
    CMD_CALIBRATION_DOWN,
    CMD_CALIBRATION_END,
    CMD_READ_CONFIG,
    CMD_REPORT_START,
    CMD_REPORT_STOP,
    CMD_ANALOG_READ,
    CMD_ID_READ,
    CMD_RGB_READ,
    CMD_FLAG_CLEAR,
    CMD_EEPROM_DISABLE,
    CMD_EEPROM_ENABLE,
    CMD_NA_0E,
    CMD_NA_0F,
    //
    CMD_RESET=0xFF,
} USART_CMD;


COM_DECLARE(USART1)

extern uint16_t USART1_TX_Count;
extern uint16_t USART1_RX_Count;

void Communication_Pack_Advanced_Key(uint8_t index,lefl_advanced_key_t *k);
void Communication_Unpack_Advanced_Key(uint8_t i);
void Communication_Pack_Key(uint8_t index,uint8_t function_key,uint16_t id);
void Communication_Unpack_Key(uint8_t i);
void Communication_Pack();
void Communication_Unpack(UART_HandleTypeDef *huart);

#endif /* COMMUNICATION_H_ */
