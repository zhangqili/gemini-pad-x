/*
 * keyboard.c
 *
 *  Created on: May 21, 2023
 *      Author: xq123
 */
#include "main.h"
#include "tim.h"
#include "usart.h"
#include "usb_device.h"
#include "analog.h"
#include "usbd_custom_hid_if.h"
#include "gpio.h"
#include "rgb.h"
#include "keyboard.h"
#include "keyboard_tree.h"
#include "MB85RC16.h"

uint8_t Keyboard_ReportBuffer[USBD_CUSTOMHID_OUTREPORT_BUF_SIZE];
lefl_bit_array_t Keyboard_KeyArray;

uint8_t Keyboard_EC11_Flag=0;
uint8_t Keyboard_Wheel_Flag=0;
uint16_t Keyboard_Advanced_SHIFT_IDs[ADVANCED_KEY_NUM];
uint16_t Keyboard_SHIFT_IDs[KEY_NUM];
uint16_t Keyboard_Advanced_ALPHA_IDs[ADVANCED_KEY_NUM];
uint16_t Keyboard_ALPHA_IDs[KEY_NUM];
uint16_t Keyboard_Tree_IDs[3][KEYBOARD_TREE_BUFFER_MAX_SIZE];
uint8_t Keyboard_TargetAdvancedKey;
lefl_key_t Keyboard_Keys[KEY_NUM];
lefl_advanced_key_t Keyboard_AdvancedKeys[ADVANCED_KEY_NUM];


void Keyboard_Init()
{
    lefl_key_attach(&(Keyboard_AdvancedKeys[0].key), KEY_EVENT_DOWN, LAMBDA(void,(void*k){lefl_loop_queue_enqueue(RGB_Argument_Queues+0, 0.0);}));
    lefl_key_attach(&(Keyboard_AdvancedKeys[1].key), KEY_EVENT_DOWN, LAMBDA(void,(void*k){lefl_loop_queue_enqueue(RGB_Argument_Queues+1, 0.0);}));
    lefl_key_attach(&(Keyboard_AdvancedKeys[2].key), KEY_EVENT_DOWN, LAMBDA(void,(void*k){lefl_loop_queue_enqueue(RGB_Argument_Queues+2, 0.0);}));
    lefl_key_attach(&(Keyboard_AdvancedKeys[3].key), KEY_EVENT_DOWN, LAMBDA(void,(void*k){lefl_loop_queue_enqueue(RGB_Argument_Queues+3, 0.0);}));

    lefl_key_attach(&KEY_SHIFT, KEY_EVENT_DOWN, NULL);
    lefl_key_attach(&KEY_ALPHA, KEY_EVENT_DOWN, NULL);
    for (uint8_t i = 0; i < ADVANCED_KEY_NUM; i++)
    {
        lefl_key_attach(&(Keyboard_AdvancedKeys[i].key), KEY_EVENT_UP, NULL);
    }
    lefl_key_attach(&(KEY_KNOB), KEY_EVENT_UP, NULL);
    lefl_key_attach(&(KEY_WHEEL), KEY_EVENT_UP, NULL);
    lefl_key_attach(&(KEY_KNOB_CLOCKWISE), KEY_EVENT_UP, NULL);
    lefl_key_attach(&(KEY_KNOB_ANTICLOCKWISE), KEY_EVENT_UP, NULL);
    lefl_key_attach(&(KEY_WHEEL_CLOCKWISE), KEY_EVENT_UP, NULL);
    lefl_key_attach(&(KEY_WHEEL_ANTICLOCKWISE), KEY_EVENT_UP, NULL);

    Keyboard_ID_Recovery();
}

void Keyboard_Scan()
{
    if(Keyboard_AdvancedKeys[0].mode==LEFL_KEY_DIGITAL_MODE)
    {
        lefl_advanced_key_update_raw(Keyboard_AdvancedKeys+0,KEY1);
    }
    if(Keyboard_AdvancedKeys[1].mode==LEFL_KEY_DIGITAL_MODE)
    {
        lefl_advanced_key_update_raw(Keyboard_AdvancedKeys+1,KEY2);
    }
    if(Keyboard_AdvancedKeys[2].mode==LEFL_KEY_DIGITAL_MODE)
    {
        lefl_advanced_key_update_raw(Keyboard_AdvancedKeys+2,KEY3);
    }
    if(Keyboard_AdvancedKeys[3].mode==LEFL_KEY_DIGITAL_MODE)
    {
        lefl_advanced_key_update_raw(Keyboard_AdvancedKeys+3,KEY4);
    }
    lefl_key_update(&KEY_SHIFT,SHIFT);
    lefl_key_update(&KEY_ALPHA,ALPHA);
    lefl_key_update(&KEY_KNOB,KNOB);
    lefl_key_update(&KEY_WHEEL,WHEEL);
    if(Keyboard_EC11_Flag)
    {
        Keyboard_EC11_Flag--;
    }
    if(Keyboard_Wheel_Flag)
    {
        Keyboard_Wheel_Flag--;
    }
    if(!Keyboard_EC11_Flag)
    {
        lefl_key_update(&KEY_KNOB_CLOCKWISE, false);
        lefl_key_update(&KEY_KNOB_ANTICLOCKWISE, false);
    }
    if(!Keyboard_Wheel_Flag)
    {
        lefl_key_update(&KEY_WHEEL_CLOCKWISE, false);
        lefl_key_update(&KEY_WHEEL_ANTICLOCKWISE, false);
    }
}

void Keyboard_SendReport()
{
    /*
    lefl_bit_array_set(&Keyboard_KeyArray, KEY1_BINDING, !(rand()%16));
    lefl_bit_array_set(&Keyboard_KeyArray, KEY2_BINDING, !(rand()%16));
    lefl_bit_array_set(&Keyboard_KeyArray, KEY3_BINDING, !(rand()%16));
    lefl_bit_array_set(&Keyboard_KeyArray, KEY4_BINDING, !(rand()%16));
    */
    memset(Keyboard_ReportBuffer,0,USBD_CUSTOMHID_OUTREPORT_BUF_SIZE);
    if(KEY_SHIFT.state)
    {
        for (uint8_t i = 0; i < ADVANCED_KEY_NUM; i++)
        {
            Keyboard_ReportBuffer[0]|= Keyboard_AdvancedKeys[i].key.state?((Keyboard_Advanced_SHIFT_IDs[i]>>8) & 0xFF):0;
            lefl_bit_array_set_or(&Keyboard_KeyArray, Keyboard_Advanced_SHIFT_IDs[i] & 0xFF, Keyboard_AdvancedKeys[i].key.state);
        }

        for (uint8_t i = 2; i < KEY_NUM; i++)
        {
            Keyboard_ReportBuffer[0]|= Keyboard_Keys[i].state?((Keyboard_SHIFT_IDs[i]>>8) & 0xFF):0;
            lefl_bit_array_set_or(&Keyboard_KeyArray, Keyboard_SHIFT_IDs[i] & 0xFF, Keyboard_Keys[i].state);
        }
        for (uint16_t i = ADVANCED_KEY_NUM+KEY_NUM; i < KEYBOARD_TREE_BUFFER_MAX_SIZE; i++)
        {
            lefl_bit_array_set_or(&Keyboard_KeyArray, Keyboard_Tree_IDs[0][i] & 0xFF, lefl_bit_array_get(&Keyboard_Tree_ReportBitmap, i));
        }
        USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS,Keyboard_ReportBuffer,USBD_CUSTOMHID_OUTREPORT_BUF_SIZE);
        return;
    }
    if(KEY_ALPHA.state)
    {
        for (uint8_t i = 0; i < ADVANCED_KEY_NUM; i++)
        {
            Keyboard_ReportBuffer[0]|= Keyboard_AdvancedKeys[i].key.state?((Keyboard_Advanced_ALPHA_IDs[i]>>8) & 0xFF):0;
            lefl_bit_array_set_or(&Keyboard_KeyArray, Keyboard_Advanced_ALPHA_IDs[i] & 0xFF, Keyboard_AdvancedKeys[i].key.state);
        }

        for (uint8_t i = 2; i < KEY_NUM; i++)
        {
            Keyboard_ReportBuffer[0]|= Keyboard_Keys[i].state?((Keyboard_ALPHA_IDs[i]>>8) & 0xFF):0;
            lefl_bit_array_set_or(&Keyboard_KeyArray, Keyboard_ALPHA_IDs[i] & 0xFF, Keyboard_Keys[i].state);
        }
        for (uint16_t i = ADVANCED_KEY_NUM+KEY_NUM; i < KEYBOARD_TREE_BUFFER_MAX_SIZE; i++)
        {
            lefl_bit_array_set_or(&Keyboard_KeyArray, Keyboard_Tree_IDs[0][i] & 0xFF, lefl_bit_array_get(&Keyboard_Tree_ReportBitmap, i));
        }
        USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS,Keyboard_ReportBuffer,USBD_CUSTOMHID_OUTREPORT_BUF_SIZE);
        return;
    }
    for (uint8_t i = 0; i < ADVANCED_KEY_NUM; i++)
    {
        Keyboard_ReportBuffer[0]|= Keyboard_AdvancedKeys[i].key.state?((Keyboard_AdvancedKeys[i].key.id>>8) & 0xFF):0;
        lefl_bit_array_set_or(&Keyboard_KeyArray, Keyboard_AdvancedKeys[i].key.id & 0xFF, Keyboard_AdvancedKeys[i].key.state);
    }
    for (uint16_t i = ADVANCED_KEY_NUM+KEY_NUM; i < KEYBOARD_TREE_BUFFER_MAX_SIZE; i++)
    {
        lefl_bit_array_set_or(&Keyboard_KeyArray, Keyboard_Tree_IDs[0][i] & 0xFF, lefl_bit_array_get(&Keyboard_Tree_ReportBitmap, i));
    }
    Keyboard_ReportBuffer[0]|=KEY_KNOB.state?((KEY_KNOB.id>>8) & 0xFF):0;
    Keyboard_ReportBuffer[0]|=KEY_WHEEL.state?((KEY_WHEEL.id>>8) & 0xFF):0;

    lefl_bit_array_set_or(&Keyboard_KeyArray, KEY_KNOB.id, KEY_KNOB.state);
    lefl_bit_array_set_or(&Keyboard_KeyArray, KEY_WHEEL.id, KEY_WHEEL.state);

    lefl_bit_array_set_or(&Keyboard_KeyArray, KEY_KNOB_CLOCKWISE.id, KEY_KNOB_CLOCKWISE.state);
    lefl_bit_array_set_or(&Keyboard_KeyArray, KEY_KNOB_ANTICLOCKWISE.id, KEY_KNOB_ANTICLOCKWISE.state);
    lefl_bit_array_set_or(&Keyboard_KeyArray, KEY_WHEEL_CLOCKWISE.id, KEY_WHEEL_CLOCKWISE.state);
    lefl_bit_array_set_or(&Keyboard_KeyArray, KEY_WHEEL_ANTICLOCKWISE.id, KEY_WHEEL_ANTICLOCKWISE.state);

    USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS,Keyboard_ReportBuffer,USBD_CUSTOMHID_OUTREPORT_BUF_SIZE);
}


void Keyboard_ID_Save()
{
    eeprom_buzy = true;
    for (uint16_t i = 0; i < ADVANCED_KEY_NUM; i++)
    {
        MB85RC16_WriteWord(KEY_SHIFT_ID_ADDRESS+i*2,Keyboard_Advanced_SHIFT_IDs[i]);
        MB85RC16_WriteWord(KEY_ALPHA_ID_ADDRESS+i*2,Keyboard_Advanced_ALPHA_IDs[i]);
    }
    for (uint16_t i = 0; i < KEY_NUM; i++)
    {
        MB85RC16_WriteWord(KEY_NORMAL_ID_ADDRESS+(i+ADVANCED_KEY_NUM)*2,Keyboard_Keys[i].id);
        MB85RC16_WriteWord(KEY_SHIFT_ID_ADDRESS+(i+ADVANCED_KEY_NUM)*2,Keyboard_SHIFT_IDs[i]);
        MB85RC16_WriteWord(KEY_ALPHA_ID_ADDRESS+(i+ADVANCED_KEY_NUM)*2,Keyboard_ALPHA_IDs[i]);
    }
    eeprom_buzy = false;
}

void Keyboard_ID_Recovery()
{
    eeprom_buzy = true;
    for (uint16_t i = 0; i < ADVANCED_KEY_NUM; i++)
    {
        MB85RC16_ReadWord(KEY_SHIFT_ID_ADDRESS+i*2,Keyboard_Advanced_SHIFT_IDs+i);
        MB85RC16_ReadWord(KEY_ALPHA_ID_ADDRESS+i*2,Keyboard_Advanced_ALPHA_IDs+i);
    }
    for (uint16_t i = 0; i < KEY_NUM; i++)
    {
        MB85RC16_ReadWord(KEY_NORMAL_ID_ADDRESS+(i+ADVANCED_KEY_NUM)*2,&(Keyboard_Keys[i].id));
        MB85RC16_ReadWord(KEY_SHIFT_ID_ADDRESS+(i+ADVANCED_KEY_NUM)*2,Keyboard_SHIFT_IDs+i);
        MB85RC16_ReadWord(KEY_ALPHA_ID_ADDRESS+(i+ADVANCED_KEY_NUM)*2,Keyboard_ALPHA_IDs+i);
    }
    eeprom_buzy = false;
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    if(htim == &htim3)
    {
        Keyboard_Wheel_Flag=6;
        if(htim->Instance->CR1==0x01)
        {
            lefl_key_update(&KEY_WHEEL_CLOCKWISE, true);
            lefl_key_update(&KEY_WHEEL_ANTICLOCKWISE, false);
        }
        if(htim->Instance->CR1==0x11)
        {
            lefl_key_update(&KEY_WHEEL_CLOCKWISE, false);
            lefl_key_update(&KEY_WHEEL_ANTICLOCKWISE, true);
        }
    }

    if(htim == &htim8)
    {
        Keyboard_EC11_Flag=6;
        if(htim->Instance->CR1==0x01)
        {
            lefl_key_update(&KEY_KNOB_CLOCKWISE, false);
            lefl_key_update(&KEY_KNOB_ANTICLOCKWISE, true);
        }
        if(htim->Instance->CR1==0x11)
        {
            lefl_key_update(&KEY_KNOB_CLOCKWISE, true);
            lefl_key_update(&KEY_KNOB_ANTICLOCKWISE, false);
        }
    }
}

