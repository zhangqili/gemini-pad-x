/*
 * fezui_config.c
 *
 *  Created on: 2023年6月19日
 *      Author: xq123
 */

#include "fezui.h"
#include "fezui_var.h"
#include "lefl.h"
#include "flash_address.h"
#include "MB85RC16.h"
#include "display.h"
#include "communication.h"
#include "rgb.h"


uint8_t tempuint;


void fezui_init()
{
    MD_OLED_RST_Set();
    u8g2_Setup_ssd1306_128x64_noname_f(&(fezui.u8g2), U8G2_R0, u8x8_byte_4wire_hw_spi,
            u8x8_stm32_gpio_and_delay);
    u8g2_InitDisplay(&(fezui.u8g2));
    u8g2_SetPowerSave(&(fezui.u8g2), 0);
    //u8g2_SetBitmapMode(&(fezui.u8g2), 1);
    u8g2_SetFontMode(&(fezui.u8g2), 1);
    u8g2_ClearBuffer(&(fezui.u8g2));
    u8g2_DrawLine(&(fezui.u8g2), 15, 15, 20, 18);
    u8g2_DrawLine(&(fezui.u8g2), 15, 22, 20, 18);
    u8g2_DrawLine(&(fezui.u8g2), 26, 16, 20, 18);
    u8g2_DrawLine(&(fezui.u8g2), 26, 26, 20, 18);
    u8g2_DrawLine(&(fezui.u8g2), 26, 16, 31, 12);
    u8g2_DrawLine(&(fezui.u8g2), 21, 8, 31, 12);
    u8g2_DrawLine(&(fezui.u8g2), 39, 4, 31, 12);
    u8g2_DrawLine(&(fezui.u8g2), 42, 21, 31, 12);
    u8g2_DrawLine(&(fezui.u8g2), 26, 26, 33, 30);
    u8g2_DrawLine(&(fezui.u8g2), 46, 39, 33, 30);
    u8g2_DrawLine(&(fezui.u8g2), 26, 26, 26, 38);
    u8g2_DrawLine(&(fezui.u8g2), 42, 44, 26, 38);
    u8g2_DrawLine(&(fezui.u8g2), 42, 21, 52, 27);
    u8g2_DrawLine(&(fezui.u8g2), 55, 26, 52, 27);
    u8g2_DrawLine(&(fezui.u8g2), 42, 21, 50, 32);
    u8g2_SendBuffer(&(fezui.u8g2));

    lefl_loop_array_init(&KPS_history, KPS_history_data, sizeof(KPS_history_data)/sizeof(lefl_array_elm_t));
    lefl_loop_array_init(&KPS_queue, KPS_queue_data, sizeof(KPS_queue_data)/sizeof(lefl_array_elm_t));
    lefl_loop_array_init(analog_historys+0, analog_history1_data, sizeof(analog_history1_data)/sizeof(lefl_array_elm_t));
    lefl_loop_array_init(analog_historys+1, analog_history2_data, sizeof(analog_history2_data)/sizeof(lefl_array_elm_t));
    lefl_loop_array_init(analog_historys+2, analog_history3_data, sizeof(analog_history3_data)/sizeof(lefl_array_elm_t));
    lefl_loop_array_init(analog_historys+3, analog_history4_data, sizeof(analog_history4_data)/sizeof(lefl_array_elm_t));

    lefl_bit_array_init(lines+0, lines1_data, sizeof(lines1_data)*8);
    lefl_bit_array_init(lines+1, lines2_data, sizeof(lines2_data)*8);
    lefl_bit_array_init(lines+2, lines3_data, sizeof(lines3_data)*8);
    lefl_bit_array_init(lines+3, lines4_data, sizeof(lines4_data)*8);

    lefl_key_attach(&(Keyboard_AdvancedKeys[0].key), KEY_EVENT_DOWN, LAMBDA(void,(void*k){fezui_keytotalcounts[0]++;key_triggered_count++;}));
    lefl_key_attach(&(Keyboard_AdvancedKeys[1].key), KEY_EVENT_DOWN, LAMBDA(void,(void*k){fezui_keytotalcounts[1]++;key_triggered_count++;}));
    lefl_key_attach(&(Keyboard_AdvancedKeys[2].key), KEY_EVENT_DOWN, LAMBDA(void,(void*k){fezui_keytotalcounts[2]++;key_triggered_count++;}));
    lefl_key_attach(&(Keyboard_AdvancedKeys[3].key), KEY_EVENT_DOWN, LAMBDA(void,(void*k){fezui_keytotalcounts[3]++;key_triggered_count++;}));

    menupage_init();
    settingspage_init();
    calibrationpage_init();
    keyconfigpage_init();
    advancedconfigpage_init();
    keylistpage_init();
    knobconfigpage_init();
    rgbconfigpage_init();
    displayconfigpage_init();

    Analog_Read();
    Keyboard_ID_Recovery();
    RGB_Recovery();
    fezui_read_counts();
    fezui_read();
    lefl_link_frame_navigate(&mainframe, &homepage);

    //fezui_reset();
}

void fezui_timer_handler()
{
    for (uint8_t i = 0; i < KEY_NUM; i++)
    {
        lefl_key_update(Keyboard_Keys+i, key_buffer[i+ADVANCED_KEY_NUM]);
    }
    lefl_link_frame_logic(&mainframe);
    fezui_cursor_move(&fezui ,&cursor, &target_cursor);

    for (uint8_t i = 0; i < ADVANCED_KEY_NUM; i++)
    {
        lefl_bit_array_shift(lines+i, 1);
        if (Keyboard_AdvancedKeys[i].key.state)
        {
            lefl_bit_array_set(lines+i, 0, true);
        }
    }
    lefl_loop_array_push_back(&KPS_queue, key_triggered_count);
    key_triggered_count=0;

    fezui_kps = 0;
    for (uint8_t i = 0; i < REFRESH_RATE; i++)
    {
        fezui_kps += KPS_queue.data[i];
    }
    if (fezui_kps > UI_KPSMaximumPerSecond)
    {
        UI_KPSMaximumPerSecond = fezui_kps;
    }
    KPS_history_max=lefl_loop_array_max(&KPS_history);
    if(fezui_kps>KPS_history_max)
    {
        KPS_history_max=fezui_kps;
        KPS_history.data[KPS_history.index]=fezui_kps;
    }
    fezui_save_counts();

    if (KPS_history_max || Keyboard_Keys[0].state||Keyboard_Keys[1].state||Keyboard_Keys[2].state||Keyboard_Keys[3].state||Keyboard_Keys[4].state||Keyboard_Keys[5].state||Keyboard_Keys[6].state||Keyboard_Keys[7].state)
    {
        fezui.screensaver_countdown = fezui.screensaver_timeout;
    }
    if(!HAL_GPIO_ReadPin(MENU_GPIO_Port, MENU_Pin))
    {
        lefl_link_frame_go_back(&mainframe);
    }
}


void fezui_render_handler()
{
    u8g2_ClearBuffer(&(fezui.u8g2));
    if(fezui.invert)
    {
        u8g2_SendF(&(fezui.u8g2), "c", 0xA7);
    }
    else
    {

    }
    u8g2_SetDrawColor(&(fezui.u8g2),1);
    lefl_link_frame_draw(&mainframe);
    if(fezui.screensaver_timeout)
    {
        if(fezui.invert)
        {
            fezui_veil(&(fezui),0,0,128,64,7-fezui.screensaver_countdown,1);
        }
        else
        {
            fezui_veil(&(fezui),0,0,128,64,7-fezui.screensaver_countdown,0);
        }
        u8g2_SetPowerSave(&(fezui.u8g2),!fezui.screensaver_countdown);
    }
#ifdef _FPS_ON
    u8g2_SetDrawColor(&(fezui.u8g2), 2);
    u8g2_SetFont(&(fezui.u8g2), fez_font_6x10_m);
    u8g2_DrawStr(&(fezui.u8g2),95+15,10,fpsstr);
#endif
    u8g2_SendBuffer(&(fezui.u8g2));
    fezui_fps++;
}


void keyid_prase(uint16_t id,char* str,uint16_t str_len)
{
    bool key_found = false;
    memset(str,0,str_len);
    lefl_bit_array_t temp_bit_array;
    lefl_bit_array_init(&temp_bit_array, (size_t*)((uint8_t*)(&id)+1), 8);
    for (uint8_t i = 0; i < 8;i++)
    {
        if(lefl_bit_array_get(&temp_bit_array, i))
        {
            if(key_found)
            {
                strcat(str, " + ");
                strcat(str, hid_usage_names[i]);
            }
            else
            {
                strcat(str, hid_usage_names[i]);
                key_found=true;
            }
        }
    }

    if(id&0xFF)
    {
        if(key_found)
        {
            strcat(str, " + ");
            strcat(str, hid_usage_names[(id&0xFF)+8]);
        }
        else
        {
            strcat(str, hid_usage_names[(id&0xFF)+8]);
            key_found=true;
        }
    }
    else
    {
        if(!key_found)
        {
            strcat(str, "None");
        }
    }
}


void Keyboard_ID_Save()
{
    if(eeprom_enable)
    {
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
    }
}

void Keyboard_ID_Recovery()
{
    if(eeprom_enable)
    {
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
    }
}

void Analog_Read()
{
    if(eeprom_enable)
    {
        for (uint16_t i = 0; i < ADVANCED_KEY_NUM; i++)
        {
            MB85RC16_ReadWord (i*64+ADVANCED_KEY_CONFIG_ADDRESS,       &(Keyboard_AdvancedKeys[i].key.id));
            MB85RC16_ReadByte (i*64+ADVANCED_KEY_CONFIG_ADDRESS+2,     &(Keyboard_AdvancedKeys[i].mode));
            MB85RC16_ReadFloat(i*64+ADVANCED_KEY_CONFIG_ADDRESS+3,     &(Keyboard_AdvancedKeys[i].trigger_distance));
            MB85RC16_ReadFloat(i*64+ADVANCED_KEY_CONFIG_ADDRESS+3+4*1, &(Keyboard_AdvancedKeys[i].release_distance));
            MB85RC16_ReadFloat(i*64+ADVANCED_KEY_CONFIG_ADDRESS+3+4*2, &(Keyboard_AdvancedKeys[i].schmitt_parameter));
            MB85RC16_ReadFloat(i*64+ADVANCED_KEY_CONFIG_ADDRESS+3+4*3, &(Keyboard_AdvancedKeys[i].trigger_speed));
            MB85RC16_ReadFloat(i*64+ADVANCED_KEY_CONFIG_ADDRESS+3+4*4, &(Keyboard_AdvancedKeys[i].release_speed));
            MB85RC16_ReadFloat(i*64+ADVANCED_KEY_CONFIG_ADDRESS+3+4*5, &(Keyboard_AdvancedKeys[i].upper_deadzone));
            MB85RC16_ReadFloat(i*64+ADVANCED_KEY_CONFIG_ADDRESS+3+4*6, &(Keyboard_AdvancedKeys[i].lower_deadzone));
            MB85RC16_ReadFloat(i*64+ADVANCED_KEY_CONFIG_ADDRESS+3+4*7, &(Keyboard_AdvancedKeys[i].range));
            MB85RC16_ReadFloat(i*64+ADVANCED_KEY_CONFIG_ADDRESS+3+4*8, &(Keyboard_AdvancedKeys[i].upper_bound));
            MB85RC16_ReadFloat(i*64+ADVANCED_KEY_CONFIG_ADDRESS+3+4*9, &(Keyboard_AdvancedKeys[i].lower_bound));
            lefl_advanced_key_set_deadzone(&Keyboard_AdvancedKeys[i], Keyboard_AdvancedKeys[i].upper_deadzone, Keyboard_AdvancedKeys[i].lower_deadzone);
        }
    }
}


void Analog_Save()
{
    if(eeprom_enable)
    {
        for (uint16_t i = 0; i < ADVANCED_KEY_NUM; i++)
        {
            MB85RC16_WriteWord (i*64+ADVANCED_KEY_CONFIG_ADDRESS,       Keyboard_AdvancedKeys[i].key.id);
            MB85RC16_WriteByte (i*64+ADVANCED_KEY_CONFIG_ADDRESS+2,     Keyboard_AdvancedKeys[i].mode);
            MB85RC16_WriteFloat(i*64+ADVANCED_KEY_CONFIG_ADDRESS+3,     Keyboard_AdvancedKeys[i].trigger_distance);
            MB85RC16_WriteFloat(i*64+ADVANCED_KEY_CONFIG_ADDRESS+3+4*1, Keyboard_AdvancedKeys[i].release_distance);
            MB85RC16_WriteFloat(i*64+ADVANCED_KEY_CONFIG_ADDRESS+3+4*2, Keyboard_AdvancedKeys[i].schmitt_parameter);
            MB85RC16_WriteFloat(i*64+ADVANCED_KEY_CONFIG_ADDRESS+3+4*3, Keyboard_AdvancedKeys[i].trigger_speed);
            MB85RC16_WriteFloat(i*64+ADVANCED_KEY_CONFIG_ADDRESS+3+4*4, Keyboard_AdvancedKeys[i].release_speed);
            MB85RC16_WriteFloat(i*64+ADVANCED_KEY_CONFIG_ADDRESS+3+4*5, Keyboard_AdvancedKeys[i].upper_deadzone);
            MB85RC16_WriteFloat(i*64+ADVANCED_KEY_CONFIG_ADDRESS+3+4*6, Keyboard_AdvancedKeys[i].lower_deadzone);
            MB85RC16_WriteFloat(i*64+ADVANCED_KEY_CONFIG_ADDRESS+3+4*7, Keyboard_AdvancedKeys[i].range);
            MB85RC16_WriteFloat(i*64+ADVANCED_KEY_CONFIG_ADDRESS+3+4*8, Keyboard_AdvancedKeys[i].upper_bound);
            MB85RC16_WriteFloat(i*64+ADVANCED_KEY_CONFIG_ADDRESS+3+4*9, Keyboard_AdvancedKeys[i].lower_bound);
        }
    }
}

void fezui_save_counts()
{
    if(eeprom_enable)
    {
        for (uint16_t i = 0; i < ADVANCED_KEY_NUM; i++)
        {
            MB85RC16_WriteLong(KEY_COUNTS_ADDRESS+i*4, fezui_keytotalcounts[i]);
        }
    }
}

void fezui_read_counts()
{
    if(eeprom_enable)
    {
        for (uint16_t i = 0; i < ADVANCED_KEY_NUM; i++)
        {
            MB85RC16_ReadLong(KEY_COUNTS_ADDRESS+i*4, fezui_keyinitcounts+i);
            fezui_keytotalcounts[i]=fezui_keyinitcounts[i];
        }
    }
}

void fezui_save()
{
    if(eeprom_enable)
    {
        MB85RC16_WriteByte(FEZUI_CONFIG_ADDRESS, fezui.invert);
        MB85RC16_WriteByte(FEZUI_CONFIG_ADDRESS+1, fezui.contrast);
        MB85RC16_WriteFloat(FEZUI_CONFIG_ADDRESS+2, fezui.speed);
        MB85RC16_WriteWord(FEZUI_CONFIG_ADDRESS+6, fezui.screensaver_timeout);
    }
}

void fezui_read()
{
    uint8_t b;
    if(eeprom_enable)
    {
        MB85RC16_ReadByte(FEZUI_CONFIG_ADDRESS, &b);
        MB85RC16_ReadByte(FEZUI_CONFIG_ADDRESS+1, &fezui.contrast);
        MB85RC16_ReadFloat(FEZUI_CONFIG_ADDRESS+2, &fezui.speed);
        MB85RC16_ReadWord(FEZUI_CONFIG_ADDRESS+6, &fezui.screensaver_timeout);
    }
    fezui.invert = b;
    fezui_apply(&fezui);
}

void fezui_reset()
{
    fezui.invert=1;
    fezui.contrast=255;
    fezui.screensaver_countdown=60;
    fezui.screensaver_timeout=60;
    fezui.speed=0.05;
    fezui_apply(&fezui);
    fezui_save();
}
