/*
 * fezui_menupage.c
 *
 *  Created on: 2023年6月19日
 *      Author: xq123
 */

#include "fezui.h"
#include "fezui_var.h"
#include "communication.h"

lefl_page_t menupage={menupage_logic,menupage_draw,menupage_load};

lefl_menu_t mainmenu;
const char* mainmenu_items[] = {"Home","Oscilloscope","Statistic","Settings"};


float menu_offset;

lefl_animation_base_t menuanimation={
        .easing_func=lefl_animation_cubic_ease,
        .from=100,
        .to=0,
        .end=100,
        .tick=0,
        .parameter1=3.0,
        .parameter2=3.0,
        .mode=LEFL_ANIMATION_EASE_OUT,
        .target=&menu_offset,
};

void menupage_init()
{
    lefl_menu_init(&mainmenu, mainmenu_items, sizeof(mainmenu_items)/sizeof(const char*), main_menu_cb);
}

void menupage_logic(void *page)
{
    lefl_animation_tick(&menuanimation);
    fezui_cursor_set(
            &target_cursor ,
            3+(int16_t)menu_offset,
            mainmenu.selected_index*ITEM_HEIGHT+3 ,
            strlen(mainmenu.items[mainmenu.selected_index])*6+6,
            ITEM_HEIGHT);
    Communication_Add8(USART1, PROTOCOL_CMD,CMD_REPORT_STOP);
    Communication_USART1_Transmit();
}
void menupage_draw(void *page)
{
    u8g2_SetFont(&(fezui.u8g2), u8g2_font_6x13_tf);
    for(uint8_t i=0;i<mainmenu.len;i++)
    {
        u8g2_DrawStr(&(fezui.u8g2),5 + (int16_t)menu_offset,ITEM_HEIGHT*(i+1),mainmenu.items[i]);
    }
    fezui_draw_cursor(&fezui, &cursor);
}

void main_menu_cb(void *menu)
{

    switch (((lefl_menu_t*)menu)->selected_index)
    {
    case 0:
        lefl_link_frame_go_back(&mainframe);
        break;
    case 1:
        lefl_link_frame_navigate(&mainframe, &oscilloscopepage);
        break;
    case 2:
    	lefl_link_frame_navigate(&mainframe, &statisticpage);
        break;
    case 3:
        lefl_link_frame_navigate(&mainframe, &settingspage);
        break;
    default:
        break;
    }
}

void menupage_load(void *page)
{
    Communication_Add8(USART1, PROTOCOL_CMD,CMD_FLAG_CLEAR);
    Communication_Add8(USART1, PROTOCOL_CMD,CMD_REPORT_STOP);
    Communication_USART1_Transmit();
    lefl_animation_begin(&menuanimation);

    lefl_key_attach(&KEY_KNOB, KEY_EVENT_DOWN, LAMBDA(void,(void*k){lefl_link_frame_go_back(&mainframe);fezui_cursor_set(&cursor ,0 ,0 ,WIDTH ,HEIGHT);}));
    lefl_key_attach(&KEY_WHEEL, KEY_EVENT_DOWN, LAMBDA(void,(void*k){lefl_menu_click(&mainmenu);}));
    lefl_key_attach(&KEY_KNOB_CLOCKWISE, KEY_EVENT_DOWN, LAMBDA(void,(void*k){lefl_menu_index_increase(&mainmenu, 1);}));
    lefl_key_attach(&KEY_KNOB_ANTICLOCKWISE, KEY_EVENT_DOWN, LAMBDA(void,(void*k){lefl_menu_index_increase(&mainmenu, -1);}));
    lefl_key_attach(&KEY_WHEEL_CLOCKWISE, KEY_EVENT_DOWN, LAMBDA(void,(void*k){lefl_menu_index_increase(&mainmenu, 1);}));
    lefl_key_attach(&KEY_WHEEL_ANTICLOCKWISE, KEY_EVENT_DOWN, LAMBDA(void,(void*k){lefl_menu_index_increase(&mainmenu, -1);}));
}

