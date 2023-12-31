/*
 * fezui_knobconfigpage.c
 *
 *  Created on: Aug 21, 2023
 *      Author: xq123
 */
#include "fezui.h"
#include "fezui_var.h"
#include "communication.h"

#define ROW_HEIGHT 16
#define SPERATOR_X 80

lefl_menu_t knobconfig_menu;

static const char* knobconfig_menu_items[] = {"Press", "Clockwise", "Anticlockwise"};
lefl_menu_t knobconfig_menu;
static char press_binding_text[128];
static char clockwise_binding_text[128];
static char anticlockwise_binding_text[128];
char current_knob_name[16];

uint16_t* current_target_knob_press_id;
uint16_t* current_target_knob_clockwise_id;
uint16_t* current_target_knob_anticlockwise_id;

static fezui_scrolling_text_t press_scrolling_text;
static fezui_scrolling_text_t clockwise_scrolling_text;
static fezui_scrolling_text_t anticlockwise_scrolling_text;

void knobconfig_menu_cb(void *m)
{
    switch (((lefl_menu_t*)m)->selected_index)
    {
        case 0:
            current_target_id = current_target_knob_press_id;
            break;
        case 1:
            current_target_id = current_target_knob_clockwise_id;
            break;
        case 2:
            current_target_id = current_target_knob_anticlockwise_id;
            break;
        default:
            break;
    }
    lefl_link_frame_navigate(&mainframe, &keylistpage);

}

void knobconfigpage_init()
{
    lefl_menu_init(&knobconfig_menu, knobconfig_menu_items, sizeof(knobconfig_menu_items)/sizeof(const char*), knobconfig_menu_cb);
}

void knobconfigpage_logic(void *page)
{

    fezui_cursor_set(
            &target_cursor,
            0,
            knobconfig_menu.selected_index*ROW_HEIGHT,
            SPERATOR_X,
            ROW_HEIGHT);
    fezui_scrolling_text_update(&press_scrolling_text);
    fezui_scrolling_text_update(&clockwise_scrolling_text);
    fezui_scrolling_text_update(&anticlockwise_scrolling_text);
}

void knobconfigpage_draw(void *page)
{
    uint8_t color = u8g2_GetDrawColor(&(fezui.u8g2));

    u8g2_DrawVLine(&(fezui.u8g2), SPERATOR_X, 0, 64);
    u8g2_DrawBox(&(fezui.u8g2), SPERATOR_X,0 ,WIDTH-SPERATOR_X, ROW_HEIGHT);
    u8g2_SetFont(&(fezui.u8g2), u8g2_font_5x8_mr);
    u8g2_SetFont(&(fezui.u8g2), u8g2_font_5x8_mr);
    for(uint8_t i=0;i<knobconfig_menu.len;i++)
    {
        u8g2_DrawStr(&(fezui.u8g2),1,ROW_HEIGHT*(i+1)-9,knobconfig_menu.items[i]);
    }
    fezui_draw_scrolling_text(&fezui, 1, ROW_HEIGHT*1-2, &press_scrolling_text);
    fezui_draw_scrolling_text(&fezui, 1, ROW_HEIGHT*2-2, &clockwise_scrolling_text);
    fezui_draw_scrolling_text(&fezui, 1, ROW_HEIGHT*3-2, &anticlockwise_scrolling_text);

    u8g2_SetDrawColor(&(fezui.u8g2), 2);
    u8g2_SetFont(&(fezui.u8g2), u8g2_font_5x8_mr);
    u8g2_DrawStr(&(fezui.u8g2), SPERATOR_X+2, 8 - 1, current_knob_name);


    u8g2_SetDrawColor(&(fezui.u8g2), color);
    fezui_draw_cursor(&fezui, &cursor);
}

void go_back_cb(void*k)
{
    Keyboard_ID_Save();
    switch (current_function_key)
    {
        case 0:
            Communication_Pack_Key(ADVANCED_KEY_NUM+KEY_KNOB_INDEX, current_function_key, KEY_KNOB.id);
            Communication_Pack_Key(ADVANCED_KEY_NUM+KEY_KNOB_CLOCKWISE_INDEX, current_function_key, KEY_KNOB_CLOCKWISE.id);
            Communication_Pack_Key(ADVANCED_KEY_NUM+KEY_KNOB_ANTICLOCKWISE_INDEX, current_function_key, KEY_KNOB_ANTICLOCKWISE.id);
            Communication_Pack_Key(ADVANCED_KEY_NUM+KEY_WHEEL_INDEX, current_function_key, KEY_WHEEL.id);
            Communication_Pack_Key(ADVANCED_KEY_NUM+KEY_WHEEL_CLOCKWISE_INDEX, current_function_key, KEY_WHEEL_CLOCKWISE.id);
            Communication_Pack_Key(ADVANCED_KEY_NUM+KEY_WHEEL_ANTICLOCKWISE_INDEX, current_function_key, KEY_WHEEL_ANTICLOCKWISE.id);
            break;
        case 1:
            Communication_Pack_Key(ADVANCED_KEY_NUM+KEY_KNOB_INDEX, current_function_key, Keyboard_SHIFT_IDs[KEY_KNOB_INDEX]);
            Communication_Pack_Key(ADVANCED_KEY_NUM+KEY_KNOB_CLOCKWISE_INDEX, current_function_key,  Keyboard_SHIFT_IDs[KEY_KNOB_CLOCKWISE_INDEX]);
            Communication_Pack_Key(ADVANCED_KEY_NUM+KEY_KNOB_ANTICLOCKWISE_INDEX, current_function_key,  Keyboard_SHIFT_IDs[KEY_KNOB_ANTICLOCKWISE_INDEX]);
            Communication_Pack_Key(ADVANCED_KEY_NUM+KEY_WHEEL_INDEX, current_function_key, Keyboard_SHIFT_IDs[KEY_WHEEL_INDEX]);
            Communication_Pack_Key(ADVANCED_KEY_NUM+KEY_WHEEL_CLOCKWISE_INDEX, current_function_key, Keyboard_SHIFT_IDs[KEY_WHEEL_CLOCKWISE_INDEX]);
            Communication_Pack_Key(ADVANCED_KEY_NUM+KEY_WHEEL_ANTICLOCKWISE_INDEX, current_function_key, Keyboard_SHIFT_IDs[KEY_WHEEL_ANTICLOCKWISE_INDEX]);
            break;
        case 2:
            Communication_Pack_Key(ADVANCED_KEY_NUM+KEY_KNOB_INDEX, current_function_key, Keyboard_ALPHA_IDs[KEY_KNOB_INDEX]);
            Communication_Pack_Key(ADVANCED_KEY_NUM+KEY_KNOB_CLOCKWISE_INDEX, current_function_key,  Keyboard_ALPHA_IDs[KEY_KNOB_CLOCKWISE_INDEX]);
            Communication_Pack_Key(ADVANCED_KEY_NUM+KEY_KNOB_ANTICLOCKWISE_INDEX, current_function_key,  Keyboard_ALPHA_IDs[KEY_KNOB_ANTICLOCKWISE_INDEX]);
            Communication_Pack_Key(ADVANCED_KEY_NUM+KEY_WHEEL_INDEX, current_function_key, Keyboard_ALPHA_IDs[KEY_WHEEL_INDEX]);
            Communication_Pack_Key(ADVANCED_KEY_NUM+KEY_WHEEL_CLOCKWISE_INDEX, current_function_key, Keyboard_ALPHA_IDs[KEY_WHEEL_CLOCKWISE_INDEX]);
            Communication_Pack_Key(ADVANCED_KEY_NUM+KEY_WHEEL_ANTICLOCKWISE_INDEX, current_function_key, Keyboard_ALPHA_IDs[KEY_WHEEL_ANTICLOCKWISE_INDEX]);
            break;
        default:
            break;
    }
    Communication_USART1_Transmit();
    lefl_link_frame_go_back(&mainframe);
}

void knobconfigpage_load(void *page)
{
    keyid_prase(*current_target_knob_press_id, press_binding_text, 128);
    fezui_scrolling_text_init(&press_scrolling_text, 78, 0.2, u8g2_font_4x6_mr, press_binding_text);

    keyid_prase(*current_target_knob_clockwise_id, clockwise_binding_text, 128);
    fezui_scrolling_text_init(&clockwise_scrolling_text, 78, 0.2, u8g2_font_4x6_mr, clockwise_binding_text);

    keyid_prase(*current_target_knob_anticlockwise_id, anticlockwise_binding_text, 128);
    fezui_scrolling_text_init(&anticlockwise_scrolling_text, 78, 0.2, u8g2_font_4x6_mr, anticlockwise_binding_text);

    lefl_key_attach(&KEY_KNOB, KEY_EVENT_DOWN, go_back_cb);
    lefl_key_attach(&KEY_WHEEL, KEY_EVENT_DOWN, LAMBDA(void,(void*k)
    {
        lefl_menu_click(&knobconfig_menu);
    }));
    lefl_key_attach(&KEY_KNOB_CLOCKWISE, KEY_EVENT_DOWN, LAMBDA(void,(void*k)
    {
        lefl_menu_index_increase(&knobconfig_menu, 1);
    }));
    lefl_key_attach(&KEY_KNOB_ANTICLOCKWISE, KEY_EVENT_DOWN, LAMBDA(void,(void*k)
    {
        lefl_menu_index_increase(&knobconfig_menu, -1);
    }));
    lefl_key_attach(&KEY_WHEEL_CLOCKWISE, KEY_EVENT_DOWN, LAMBDA(void,(void*k)
    {
        lefl_menu_index_increase(&knobconfig_menu, 1);
    }));
    lefl_key_attach(&KEY_WHEEL_ANTICLOCKWISE, KEY_EVENT_DOWN, LAMBDA(void,(void*k)
    {
        lefl_menu_index_increase(&knobconfig_menu, -1);
    }));
}

lefl_page_t knobconfigpage={knobconfigpage_logic,knobconfigpage_draw,knobconfigpage_load};


