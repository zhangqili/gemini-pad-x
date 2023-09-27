/*
 * keyboard_tree.c
 *
 *  Created on: Sep 14, 2023
 *      Author: xq123
 */
#include "keyboard_tree.h"

#ifdef KEYBOARD_TREE_ON
#include "main.h"
#include "gpio.h"
#include "spi.h"
#endif
//#include "stdlib.h"

#define TIMEOUT 5

void DWT_Delay_us(volatile uint32_t microseconds);

Keyboard_TNode_Type Keyboard_Tree_BaseType;
Keyboard_TNode_Status Keyboard_Tree_BaseStatus;
Keyboard_TNode Keyboard_TNodes[KEYBOARD_CHILD_NUM];
static lefl_bit_array_unit_t Keyboard_Tree_BitmapBuffer[KEYBOARD_CHILD_NUM][KEYBOARD_CHILD_KEY_MAX_NUM/(8*sizeof(lefl_bit_array_unit_t))];
static lefl_bit_array_unit_t Keyboard_Tree_ReportBuffer[KEYBOARD_TREE_BUFFER_MAX_SIZE/(8*sizeof(lefl_bit_array_unit_t))];
lefl_bit_array_t Keyboard_Tree_ReportBitmap;

#ifdef KEYBOARD_MASTER_ON
uint8_t Master_SPI_TxBuffer[16];
uint8_t Master_SPI_RxBuffer[16];
#endif

#ifdef KEYBOARD_SLAVE_ON
uint8_t Slave_SPI_TxBuffer[16];
uint8_t Slave_SPI_RxBuffer[16];
#endif


void Keyboard_Tree_Init()
{
    Keyboard_Tree_BaseType=KEYBOARD_TNODE_ROOT;
    Keyboard_Tree_BaseStatus=KEYBOARD_TNODE_CONNECTED;
    Master_SPI_TxBuffer[0]=0x11;
    Slave_SPI_TxBuffer[0]=Keyboard_Tree_BaseStatus;
    for (uint8_t i = 0; i< KEYBOARD_CHILD_NUM; i++)
    {
        lefl_bit_array_init(&(Keyboard_TNodes[i].Bitmap), Keyboard_Tree_BitmapBuffer[i], 0);
    }
    lefl_bit_array_init(&Keyboard_Tree_ReportBitmap, Keyboard_Tree_ReportBuffer, ADVANCED_KEY_NUM+KEY_NUM);
    HAL_SPI_TransmitReceive_IT(&SPI_SLAVE, &Keyboard_Tree_BaseStatus, Slave_SPI_RxBuffer, 1);
}

#ifdef KEYBOARD_MASTER_ON
void Keyboard_Tree_Scan()
{

    if(Keyboard_TNodes[0].Bitmap.len)
    {
        HAL_GPIO_WritePin(SPI2_CS1_GPIO_Port, SPI2_CS1_Pin, GPIO_PIN_RESET);
        HAL_SPI_TransmitReceive(&SPI_MASTER, Master_SPI_TxBuffer, (uint8_t*)(Keyboard_TNodes[0].Bitmap.data), (Keyboard_TNodes[0].Bitmap.len-1)/8+1,TIMEOUT);
        HAL_GPIO_WritePin(SPI2_CS1_GPIO_Port, SPI2_CS1_Pin, GPIO_PIN_SET);
    }
    if(Keyboard_TNodes[1].Bitmap.len)
    {
        HAL_GPIO_WritePin(SPI2_CS2_GPIO_Port, SPI2_CS2_Pin, GPIO_PIN_RESET);
        HAL_SPI_TransmitReceive(&SPI_MASTER, Master_SPI_TxBuffer, (uint8_t*)(Keyboard_TNodes[1].Bitmap.data), (Keyboard_TNodes[1].Bitmap.len-1)/8+1,TIMEOUT);
        HAL_GPIO_WritePin(SPI2_CS2_GPIO_Port, SPI2_CS2_Pin, GPIO_PIN_SET);
    }
    if(Keyboard_TNodes[2].Bitmap.len)
    {
        HAL_GPIO_WritePin(SPI2_CS3_GPIO_Port, SPI2_CS3_Pin, GPIO_PIN_RESET);
        HAL_SPI_TransmitReceive(&SPI_MASTER, Master_SPI_TxBuffer, (uint8_t*)(Keyboard_TNodes[2].Bitmap.data), (Keyboard_TNodes[2].Bitmap.len-1)/8+1,TIMEOUT);
        HAL_GPIO_WritePin(SPI2_CS3_GPIO_Port, SPI2_CS3_Pin, GPIO_PIN_SET);
    }
    if(Keyboard_TNodes[3].Bitmap.len)
    {
        HAL_GPIO_WritePin(SPI2_CS4_GPIO_Port, SPI2_CS4_Pin, GPIO_PIN_RESET);
        HAL_SPI_TransmitReceive(&SPI_MASTER, Master_SPI_TxBuffer, (uint8_t*)(Keyboard_TNodes[3].Bitmap.data), (Keyboard_TNodes[3].Bitmap.len-1)/8+1,TIMEOUT);
        HAL_GPIO_WritePin(SPI2_CS4_GPIO_Port, SPI2_CS4_Pin, GPIO_PIN_SET);
    }
}

extern uint8_t count;
void Keyboard_Tree_Detect()
{
    for (uint8_t i = 0; i < 5; i++)
    {
        HAL_GPIO_WritePin(SPI2_CS1_GPIO_Port, SPI2_CS1_Pin, GPIO_PIN_RESET);
        HAL_SPI_TransmitReceive(&SPI_MASTER, Master_SPI_TxBuffer, (uint8_t*)(&(Keyboard_TNodes[0].Status)), 1, TIMEOUT);
        HAL_GPIO_WritePin(SPI2_CS1_GPIO_Port, SPI2_CS1_Pin, GPIO_PIN_SET);
        count++;
        if(Keyboard_TNodes[0].Status!=KEYBOARD_TNODE_READY)
        {
            for (uint8_t j = 0; j < 5; j++)
            {
                HAL_GPIO_WritePin(SPI2_CS1_GPIO_Port, SPI2_CS1_Pin, GPIO_PIN_RESET);
                HAL_SPI_TransmitReceive(&SPI_MASTER, Master_SPI_TxBuffer, (uint8_t*)(&(Keyboard_TNodes[0].Status)), 1, TIMEOUT);
                HAL_GPIO_WritePin(SPI2_CS1_GPIO_Port, SPI2_CS1_Pin, GPIO_PIN_SET);
                DWT_Delay_us(500);
                if(Keyboard_TNodes[0].Status==KEYBOARD_TNODE_READY)
                {
                    Keyboard_TNodes[0].Status=KEYBOARD_TNODE_ACTIVE;
                    HAL_GPIO_WritePin(SPI2_CS1_GPIO_Port, SPI2_CS1_Pin, GPIO_PIN_RESET);
                    HAL_SPI_TransmitReceive(&SPI_MASTER, Master_SPI_TxBuffer, (uint8_t*)(&(Keyboard_TNodes[0].Bitmap.len)), 2, TIMEOUT);
                    HAL_GPIO_WritePin(SPI2_CS1_GPIO_Port, SPI2_CS1_Pin, GPIO_PIN_SET);
                    Keyboard_Tree_ReportBitmap.len+=Keyboard_TNodes[0].Bitmap.len;
                    break;
                }
            }
            break;
        }
        DWT_Delay_us(50);
    }


    for (uint8_t i = 0; i < 5; i++)
    {
        HAL_GPIO_WritePin(SPI2_CS2_GPIO_Port, SPI2_CS2_Pin, GPIO_PIN_RESET);
        HAL_SPI_TransmitReceive(&SPI_MASTER, Master_SPI_TxBuffer, (uint8_t*)(&(Keyboard_TNodes[1].Status)), 1, TIMEOUT);
        HAL_GPIO_WritePin(SPI2_CS2_GPIO_Port, SPI2_CS2_Pin, GPIO_PIN_SET);
        if(Keyboard_TNodes[1].Status==KEYBOARD_TNODE_READY)
        {
            Keyboard_TNodes[1].Status=KEYBOARD_TNODE_ACTIVE;
            HAL_GPIO_WritePin(SPI2_CS2_GPIO_Port, SPI2_CS2_Pin, GPIO_PIN_RESET);
            HAL_SPI_TransmitReceive(&SPI_MASTER, Master_SPI_TxBuffer, (uint8_t*)(&(Keyboard_TNodes[1].Bitmap.len)), 2, TIMEOUT);
            HAL_GPIO_WritePin(SPI2_CS2_GPIO_Port, SPI2_CS2_Pin, GPIO_PIN_SET);
            Keyboard_Tree_ReportBitmap.len+=Keyboard_TNodes[1].Bitmap.len;
            break;
        }
        DWT_Delay_us(50);
    }


    for (uint8_t i = 0; i < 5; i++)
    {
        HAL_GPIO_WritePin(SPI2_CS3_GPIO_Port, SPI2_CS3_Pin, GPIO_PIN_RESET);
        HAL_SPI_TransmitReceive(&SPI_MASTER, Master_SPI_TxBuffer, (uint8_t*)(&(Keyboard_TNodes[2].Status)), 1, TIMEOUT);
        HAL_GPIO_WritePin(SPI2_CS3_GPIO_Port, SPI2_CS3_Pin, GPIO_PIN_SET);
        if(Keyboard_TNodes[2].Status==KEYBOARD_TNODE_READY)
        {
            Keyboard_TNodes[2].Status=KEYBOARD_TNODE_ACTIVE;
            HAL_GPIO_WritePin(SPI2_CS3_GPIO_Port, SPI2_CS3_Pin, GPIO_PIN_RESET);
            HAL_SPI_TransmitReceive(&SPI_MASTER, Master_SPI_TxBuffer, (uint8_t*)(&(Keyboard_TNodes[2].Bitmap.len)), 2, TIMEOUT);
            HAL_GPIO_WritePin(SPI2_CS3_GPIO_Port, SPI2_CS3_Pin, GPIO_PIN_SET);
            Keyboard_Tree_ReportBitmap.len+=Keyboard_TNodes[2].Bitmap.len;
            break;
        }
        DWT_Delay_us(50);
    }


    for (uint8_t i = 0; i < 5; i++)
    {
        HAL_GPIO_WritePin(SPI2_CS4_GPIO_Port, SPI2_CS4_Pin, GPIO_PIN_RESET);
        HAL_SPI_TransmitReceive(&SPI_MASTER, Master_SPI_TxBuffer, (uint8_t*)(Keyboard_TNodes[3].Status), 1, TIMEOUT);
        HAL_GPIO_WritePin(SPI2_CS4_GPIO_Port, SPI2_CS4_Pin, GPIO_PIN_SET);
        if(Keyboard_TNodes[3].Status==KEYBOARD_TNODE_READY)
        {
            Keyboard_TNodes[3].Status=KEYBOARD_TNODE_ACTIVE;
            HAL_GPIO_WritePin(SPI2_CS4_GPIO_Port, SPI2_CS4_Pin, GPIO_PIN_RESET);
            HAL_SPI_TransmitReceive(&SPI_MASTER, Master_SPI_TxBuffer, (uint8_t*)(&(Keyboard_TNodes[3].Bitmap.len)), 2, TIMEOUT);
            HAL_GPIO_WritePin(SPI2_CS4_GPIO_Port, SPI2_CS4_Pin, GPIO_PIN_SET);
            Keyboard_Tree_ReportBitmap.len+=Keyboard_TNodes[3].Bitmap.len;
            break;
        }
        DWT_Delay_us(50);
    }
    Keyboard_Tree_BaseStatus=KEYBOARD_TNODE_READY;
}
#endif

#ifdef KEYBOARD_SLAVE_ON
void Keyboard_Tree_Report()
{
    int16_t templen=ADVANCED_KEY_NUM+KEY_NUM;
    for (uint8_t i = 0; i < ADVANCED_KEY_NUM; i++)
    {
        lefl_bit_array_set(&Keyboard_Tree_ReportBitmap, i, Keyboard_AdvancedKeys[i].key.state);
    }
    for (uint8_t i = 0; i < KEY_NUM; i++)
    {
        lefl_bit_array_set(&Keyboard_Tree_ReportBitmap, i+ADVANCED_KEY_NUM, Keyboard_Keys[i].state);
    }
    for (uint8_t i = 0; i < KEYBOARD_CHILD_NUM; i++)
    {
        lefl_bit_array_copy(&Keyboard_Tree_ReportBitmap, templen, &(Keyboard_TNodes[i].Bitmap), 0, Keyboard_TNodes[i].Bitmap.len);
        templen+=Keyboard_TNodes[i].Bitmap.len;
    }
}

void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{
    if (hspi == &SPI_MASTER)
    {
    }
    if (hspi == &SPI_SLAVE)
    {
        Keyboard_Tree_BaseType=KEYBOARD_TNODE_INTERNAL;
        switch(Keyboard_Tree_BaseStatus)
        {
            case KEYBOARD_TNODE_CONNECTED:
                Keyboard_Tree_BaseStatus=KEYBOARD_TNODE_DETECT;
                HAL_SPI_TransmitReceive_IT(&SPI_SLAVE, &Keyboard_Tree_BaseStatus, Slave_SPI_RxBuffer, 1);
                break;
            case KEYBOARD_TNODE_DETECT:
                Keyboard_Tree_BaseStatus=KEYBOARD_TNODE_DETECT;
                HAL_SPI_TransmitReceive_IT(&SPI_SLAVE, &Keyboard_Tree_BaseStatus, Slave_SPI_RxBuffer, 1);
                break;
            case KEYBOARD_TNODE_READY:
                count++;
                Keyboard_Tree_BaseStatus=KEYBOARD_TNODE_ACTIVE;
                HAL_SPI_TransmitReceive_IT(&SPI_SLAVE, (uint8_t*)(&(Keyboard_Tree_ReportBitmap.len)), Slave_SPI_RxBuffer, 2);
                break;
            case KEYBOARD_TNODE_ACTIVE:
                HAL_SPI_TransmitReceive_IT(&SPI_SLAVE, (uint8_t*)(Keyboard_Tree_ReportBitmap.data), Slave_SPI_RxBuffer, (Keyboard_Tree_ReportBitmap.len-1)/8+1);
            default:
                break;
        }
    }
}
#endif


