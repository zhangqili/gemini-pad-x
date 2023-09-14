/*
 * MB85RC16.c
 *
 *  Created on: May 22, 2023
 *      Author: xq123
 */
#include "i2c.h"
#include "MB85RC16.h"

uint8_t fram_buf[16];
uint16_t fram_address;

uint8_t MB85RC16_ReadArray (uint16_t framAddr, uint8_t length, uint8_t value[])
{
    MB85RC16_SetAddress(framAddr);
    MB85RC16_I2CAddressAdapt(framAddr,0);
    HAL_I2C_Master_Receive(&FRAM_HI2C, fram_address, value, length, 10);
    return 0;
}

uint8_t MB85RC16_WriteArray (uint16_t framAddr, uint8_t length, uint8_t value[])
{
    MB85RC16_I2CAddressAdapt(framAddr,0);
    for(uint8_t i=0;i<length;i++)
    {
        fram_buf[i+1]=value[i];
    }
    HAL_I2C_Master_Transmit(&FRAM_HI2C, fram_address, fram_buf, length+1, 10);
    return 0;
}

uint8_t MB85RC16_ReadByte (uint16_t framAddr, uint8_t *value)
{
    MB85RC16_ReadArray(framAddr, 1, value);
    return 0;
}

uint8_t MB85RC16_WriteByte (uint16_t framAddr, uint8_t value)
{
    MB85RC16_WriteArray(framAddr,1,&value);
    return 0;
}

uint8_t MB85RC16_ReadWord(uint16_t framAddr, uint16_t *value)
{
    MB85RC16_ReadArray(framAddr, 2, (uint8_t*)value);
    return 0;
}
uint8_t MB85RC16_WriteWord(uint16_t framAddr, uint16_t value)
{
    MB85RC16_WriteArray(framAddr,2,(uint8_t*)&value);
    return 0;
}

uint8_t MB85RC16_ReadLong(uint16_t framAddr, uint32_t *value)
{
    MB85RC16_ReadArray(framAddr, 4, (uint8_t*)value);
    return 0;
}

uint8_t MB85RC16_WriteLong(uint16_t framAddr, uint32_t value)
{
    MB85RC16_WriteArray(framAddr,4,(uint8_t*)&value);
    return 0;
}

uint8_t MB85RC16_ReadFloat(uint16_t framAddr, float *value)
{
    MB85RC16_ReadArray(framAddr, 4, (uint8_t*)value);
    return 0;
}

uint8_t MB85RC16_WriteFloat(uint16_t framAddr, float value)
{
    MB85RC16_WriteArray(framAddr,4,(uint8_t*)&value);
    return 0;
}

