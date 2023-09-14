/**
 * Copyright (c) 2015 - present LibDriver All rights reserved
 * 
 * The MIT License (MIT)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE. 
 *
 * @file      driver_w25qxx_interface_template.c
 * @brief     driver w25qxx interface template source file
 * @version   1.0.0
 * @author    Shifeng Li
 * @date      2021-07-15
 *
 * <h3>history</h3>
 * <table>
 * <tr><th>Date        <th>Version  <th>Author      <th>Description
 * <tr><td>2021/07/15  <td>1.0      <td>Shifeng Li  <td>first upload
 * </table>
 */

#include "driver_w25qxx_interface.h"
#include "main.h"
#include "spi.h"
/**
 * @brief      spi bus write read
 * @param[in]  *in_buf points to an input buffer
 * @param[in]  in_len is the input length
 * @param[out] *out_buf points to an output buffer
 * @param[in]  out_len is the output length
 * @return     status code
 *             - 0 success
 *             - 1 write read failed
 * @note       none
 */
static uint8_t spi_write_read(uint8_t *in_buf, uint32_t in_len, uint8_t *out_buf, uint32_t out_len)
{
    uint8_t res;

    /* set cs low */
    HAL_GPIO_WritePin(FLASH_CS_GPIO_Port, FLASH_CS_Pin, GPIO_PIN_RESET);

    /* if in_len > 0 */
    if (in_len > 0)
    {
        /* transmit the input buffer */
        res = HAL_SPI_Transmit(&hspi2, in_buf, in_len, 1000);
        if (res != HAL_OK)
        {
            /* set cs high */
            HAL_GPIO_WritePin(FLASH_CS_GPIO_Port, FLASH_CS_Pin, GPIO_PIN_SET);

            return 1;
        }
    }

    /* if out_len > 0 */
    if (out_len > 0)
    {
        /* transmit to the output buffer */
        res = HAL_SPI_Receive(&hspi2, out_buf, out_len, 1000);
        if (res != HAL_OK)
        {
            /* set cs high */
            HAL_GPIO_WritePin(FLASH_CS_GPIO_Port, FLASH_CS_Pin, GPIO_PIN_SET);

            return 1;
        }
    }

    /* set cs high */
    HAL_GPIO_WritePin(FLASH_CS_GPIO_Port, FLASH_CS_Pin, GPIO_PIN_SET);

    return 0;
}

/**
 * @brief  interface spi qspi bus init
 * @return status code
 *         - 0 success
 *         - 1 spi qspi init failed
 * @note   none
 */
uint8_t w25qxx_interface_spi_qspi_init(void)
{
    return 0;
}

/**
 * @brief  interface spi qspi bus deinit
 * @return status code
 *         - 0 success
 *         - 1 spi qspi deinit failed
 * @note   none
 */
uint8_t w25qxx_interface_spi_qspi_deinit(void)
{
    return 0;
}

/**
 * @brief      interface spi qspi bus write read
 * @param[in]  instruction is the sent instruction
 * @param[in]  instruction_line is the instruction phy lines
 * @param[in]  address is the register address
 * @param[in]  address_line is the address phy lines
 * @param[in]  address_len is the address length
 * @param[in]  alternate is the register address
 * @param[in]  alternate_line is the alternate phy lines
 * @param[in]  alternate_len is the alternate length
 * @param[in]  dummy is the dummy cycle
 * @param[in]  *in_buf points to a input buffer
 * @param[in]  in_len is the input length
 * @param[out] *out_buf points to a output buffer
 * @param[in]  out_len is the output length
 * @param[in]  data_line is the data phy lines
 * @return     status code
 *             - 0 success
 *             - 1 write read failed
 * @note       none
 */
uint8_t w25qxx_interface_spi_qspi_write_read(uint8_t instruction, uint8_t instruction_line,
                                             uint32_t address, uint8_t address_line, uint8_t address_len,
                                             uint32_t alternate, uint8_t alternate_line, uint8_t alternate_len,
                                             uint8_t dummy, uint8_t *in_buf, uint32_t in_len,
                                             uint8_t *out_buf, uint32_t out_len, uint8_t data_line)
{
	if ((instruction_line != 0) || (address_line != 0) || (alternate_line != 0) || (dummy != 0) || (data_line != 1))
	{
		return 1;
	}

	return spi_write_read(in_buf, in_len, out_buf, out_len);
}

/**
 * @brief     interface delay ms
 * @param[in] ms
 * @note      none
 */
void w25qxx_interface_delay_ms(uint32_t ms)
{
	HAL_Delay(ms);
}

/**
 * @brief     interface delay us
 * @param[in] us
 * @note      none
 */
void w25qxx_interface_delay_us(uint32_t us)
{
	DWT_Delay_us(us);
}

/**
 * @brief     interface print format data
 * @param[in] fmt is the format data
 * @note      none
 */
void w25qxx_interface_debug_print(const char *const fmt, ...)
{
    
}
