/*
 * MB85RC16.h
 *
 *  Created on: May 22, 2023
 *      Author: xq123
 */

#ifndef MB85RC16_H_
#define MB85RC16_H_

#include "main.h"
#include "flash_address.h"

#define FRAM_BUF fram_buf
#define FRAM_ADDRESS fram_address
#define FRAM_HI2C hi2c1

#define MB85RC16_DEVICE_TYPE_CODE 0xA
#define MB85RC16_I2CAddressAdapt(a,b) FRAM_ADDRESS=(0xA<<4)|((a>>7)&0xE)|(b&1);\
                                      FRAM_BUF[0]=a&0xFF;
#define MB85RC16_SetAddress(a)  MB85RC16_I2CAddressAdapt(a,0);\
                                HAL_I2C_Master_Transmit(&FRAM_HI2C, fram_address, fram_buf, 1, 10);

extern bool eeprom_buzy;

uint8_t MB85RC16_ReadArray (uint16_t framAddr, uint8_t length, uint8_t value[]);
uint8_t MB85RC16_WriteArray (uint16_t framAddr, uint8_t length, uint8_t value[]);
uint8_t MB85RC16_ReadByte (uint16_t framAddr, uint8_t *value);
uint8_t MB85RC16_WriteByte (uint16_t framAddr, uint8_t value);
uint8_t MB85RC16_ReadWord(uint16_t framAddr, uint16_t *value);
uint8_t MB85RC16_WriteWord(uint16_t framAddr, uint16_t value);
uint8_t MB85RC16_ReadLong(uint16_t framAddr, uint32_t *value);
uint8_t MB85RC16_WriteLong(uint16_t framAddr, uint32_t value);
uint8_t MB85RC16_ReadFloat(uint16_t framAddr, float *value);
uint8_t MB85RC16_WriteFloat(uint16_t framAddr, float value);

#endif /* MB85RC16_H_ */
