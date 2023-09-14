/*
 * display.h
 *
 *  Created on: 2023年5月23日
 *      Author: xq123
 */

#ifndef DISPLAY_H_
#define DISPLAY_H_

#include "main.h"
#include "u8g2.h"

#define MD_OLED_RST_Clr()     HAL_GPIO_WritePin(OLED_RES_GPIO_Port,OLED_RES_Pin,GPIO_PIN_RESET)
#define MD_OLED_RST_Set()     HAL_GPIO_WritePin(OLED_RES_GPIO_Port,OLED_RES_Pin,GPIO_PIN_SET)

uint8_t u8x8_byte_4wire_hw_spi(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int,void *arg_ptr);
uint8_t u8x8_stm32_gpio_and_delay(U8X8_UNUSED u8x8_t *u8x8, U8X8_UNUSED uint8_t msg, U8X8_UNUSED uint8_t arg_int, U8X8_UNUSED void *arg_ptr);


#endif /* DISPLAY_H_ */
