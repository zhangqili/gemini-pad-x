/*
 * fezui_displayconfigpage.c
 *
 *  Created on: Aug 24, 2023
 *      Author: xq123
 */
#include "fezui.h"
#include "fezui_var.h"
#include "communication.h"

#define ROW_HEIGHT 16

lefl_menu_t displayconfig_menu;

static const char* displayconfig_menu_items[] = {"Invert","Contrast","Speed","Screen Saver Timeout"};

static fezui_scrolling_text_t scrolling_text;
static bool configing;

void displayconfigpage_init()
{
    lefl_menu_init(&displayconfig_menu, displayconfig_menu_items, sizeof(displayconfig_menu_items)/sizeof(const char*), NULL);
}

void displayconfigpage_logic(void *page)
{

    if(configing)
    {
        fezui_cursor_set(
                &target_cursor ,
                WIDTH-16,
                displayconfig_menu.selected_index*ROW_HEIGHT+ROW_HEIGHT/2,
                16,
                ROW_HEIGHT/2);
    }
    else
    {
        fezui_cursor_set(
                &target_cursor ,
                0,
                displayconfig_menu.selected_index*ROW_HEIGHT,
                128,
                ROW_HEIGHT);
    }
    fezui_scrolling_text_update(&scrolling_text);
}

void displayconfigpage_draw(void *page)
{
    u8g2_SetFont(&(fezui.u8g2), u8g2_font_5x8_mr);
    uint8_t font_width = u8g2_GetMaxCharWidth(&(fezui.u8g2));

    for(uint8_t i=0;i<displayconfig_menu.len;i++)
    {
        u8g2_DrawStr(&(fezui.u8g2),1,ROW_HEIGHT*(i+1)-9,displayconfig_menu.items[i]);
    }
    if(fezui.invert)
        u8g2_DrawStr(&(fezui.u8g2), WIDTH - strlen("ON")*font_width, ROW_HEIGHT*1-1, "ON");
    else
        u8g2_DrawStr(&(fezui.u8g2), WIDTH - strlen("OFF")*font_width, ROW_HEIGHT*1-1, "OFF");

    sprintf(fezui_buffer,"%d",fezui.contrast);
    u8g2_DrawStr(&(fezui.u8g2), WIDTH - strlen(fezui_buffer)*font_width, ROW_HEIGHT*2-1, fezui_buffer);

    sprintf(fezui_buffer,"%3.0f",fezui.speed*100);
    u8g2_DrawStr(&(fezui.u8g2), WIDTH - strlen(fezui_buffer)*font_width, ROW_HEIGHT*3-1, fezui_buffer);

    sprintf(fezui_buffer,"%3d",fezui.screensaver_timeout);
    u8g2_DrawStr(&(fezui.u8g2), WIDTH - strlen(fezui_buffer)*font_width, ROW_HEIGHT*4-1, fezui_buffer);

    //u8g2_SetFont(&(fezui.u8g2), u8g2_font_5x8_mr);
    //u8g2_SetDrawColor(&(fezui.u8g2), 2);
    //u8g2_SetDrawColor(&(fezui.u8g2), color);
    fezui_draw_cursor(&fezui, &cursor);
    fezui_apply(&fezui);
}

static void key_up_cb(void *k)
{

    if(configing)
    {
        switch (displayconfig_menu.selected_index)
        {
            case 0:
                VAR_LOOP_INCREMENT(fezui.invert,0,1,1);
                break;
            case 1:
                VAR_LOOP_INCREMENT(fezui.contrast,0,255,1);
                break;
            case 2:
                VAR_LOOP_INCREMENT(fezui.speed,0.01,1.0,0.01);
                break;
            case 3:
                VAR_LOOP_INCREMENT(fezui.screensaver_timeout,0,600,1);
                break;
            default:
                break;
        }
    }
    else
    {
        lefl_menu_index_increase(&displayconfig_menu, 1);
    }
}

static void key_down_cb(void *k)
{
    if(configing)
    {
        switch (displayconfig_menu.selected_index)
        {
            case 0:
                VAR_LOOP_DECREMENT(fezui.invert,0,1,1);
                break;
            case 1:
                VAR_LOOP_DECREMENT(fezui.contrast,0,255,1);
                break;
            case 2:
                VAR_LOOP_DECREMENT(fezui.speed,0.01,1.0,0.01);
                break;
            case 3:
                VAR_LOOP_DECREMENT(fezui.screensaver_timeout,0,600,1);
                break;
            default:
                break;
        }
    }
    else
    {
        lefl_menu_index_increase(&displayconfig_menu, -1);
    }
}

void displayconfigpage_load(void *page)
{
    lefl_key_attach(&KEY_KNOB, KEY_EVENT_DOWN, LAMBDA(void,(void*k)
    {
        if(configing)
        {
            configing=false;
        }
        else
        {
            fezui_save();
            lefl_link_frame_go_back(&mainframe);
        }
    }));
    lefl_key_attach(&KEY_WHEEL, KEY_EVENT_DOWN, LAMBDA(void,(void*k)
    {
        configing = !configing;
    }));
    lefl_key_attach(&KEY_KNOB_CLOCKWISE, KEY_EVENT_DOWN, key_up_cb);
    lefl_key_attach(&KEY_KNOB_ANTICLOCKWISE, KEY_EVENT_DOWN, key_down_cb);
    lefl_key_attach(&KEY_WHEEL_CLOCKWISE, KEY_EVENT_DOWN, key_up_cb);
    lefl_key_attach(&KEY_WHEEL_ANTICLOCKWISE, KEY_EVENT_DOWN, key_down_cb);
}

lefl_page_t displayconfigpage={displayconfigpage_logic,displayconfigpage_draw, displayconfigpage_load};

