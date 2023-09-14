/*
 * fezui_keylistpage.c
 *
 *  Created on: Aug 20, 2023
 *      Author: xq123
 */


/*
 * fezui_keyselectpage.c
 *
 *  Created on: 2023年6月19日
 *      Author: xq123
 */
#include "fezui.h"
#include "fezui_var.h"

#define ROW_HEIGHT 8

static fezui_scrollview_t scrollview = { .content_height = ROW_HEIGHT*sizeof(hid_usage_names)/sizeof(const char*), .content_width =
        128, .abscissa = 0, .ordinate = 0 };

lefl_menu_t keylist;
static float target_ordinate=0;
uint16_t* current_target_id;

lefl_page_t keylistpage = { keylistpage_logic, keylistpage_draw, keylistpage_load };

static lefl_bit_array_t head_key_usage;

void keylist_cb(void *m)
{
    if(((lefl_menu_t*)m)->selected_index<8)
    {
        lefl_bit_array_set(&head_key_usage, ((lefl_menu_t*)m)->selected_index, !lefl_bit_array_get(&head_key_usage, ((lefl_menu_t*)m)->selected_index));
    }
    else
    {
        *current_target_id = (*current_target_id & 0xFF00 ) | ((((lefl_menu_t*)m)->selected_index-8)&0xFF);
    }
}

void keylistpage_init()
{
    lefl_menu_init(&keylist, hid_usage_names, sizeof(hid_usage_names)/sizeof(const char*), keylist_cb);
}

void keylistpage_logic(void *page)
{
    fezui_cursor_set(
        &target_cursor ,
        0,
        keylist.selected_index*ROW_HEIGHT - (u8g2_int_t)scrollview.ordinate,
        strlen(keylist.items[keylist.selected_index])*5+2,
        ROW_HEIGHT);
    if((keylist.selected_index+1)*ROW_HEIGHT-target_ordinate>64)
    {
        target_ordinate = (keylist.selected_index+1)*ROW_HEIGHT-64;
    }
    if((keylist.selected_index)*ROW_HEIGHT<target_ordinate)
    {
        target_ordinate = (keylist.selected_index)*ROW_HEIGHT;
    }
    CONVERGE_TO_ROUNDED(scrollview.ordinate, target_ordinate, fezui.speed);
}
void keylistpage_draw(void *page)
{
    u8g2_SetFont(&(fezui.u8g2), u8g2_font_5x8_mr);
    for(uint8_t i=0;i<keylist.len;i++)
    {
        u8g2_DrawStr(&(fezui.u8g2),1 ,ROW_HEIGHT*(i+1)-1 - (u8g2_int_t)scrollview.ordinate, keylist.items[i]);
        u8g2_DrawFrame(&(fezui.u8g2),116,ROW_HEIGHT*i + 1 - (u8g2_int_t)scrollview.ordinate,6,6);
    }
    for(uint8_t i=0;i<8;i++)
    {
        if(lefl_bit_array_get(&head_key_usage, i))
        {
            u8g2_DrawBox(&(fezui.u8g2),116,ROW_HEIGHT*i + 1 - (u8g2_int_t)scrollview.ordinate,6,6);
        }
    }
    //sprintf(fezui_buffer,"%d",current_target_key->id&0xFF);
    //u8g2_DrawStr(&(fezui.u8g2), 64, 32, fezui_buffer);

    u8g2_DrawBox(&(fezui.u8g2),116,ROW_HEIGHT*(((*current_target_id)&0xFF)+8) + 1 - (u8g2_int_t)scrollview.ordinate,6,6);
    fezui_draw_scrollview(&fezui, 0, 0, WIDTH, HEIGHT, &scrollview);
    fezui_draw_cursor(&fezui, &cursor);
}

void keylistpage_load(void *page)
{
    lefl_bit_array_init(&head_key_usage, (size_t*)(((uint8_t*)current_target_id)+1), 8);
    lefl_key_attach(&KEY_KNOB, KEY_EVENT_DOWN, LAMBDA(void,(void*k){lefl_link_frame_go_back(&mainframe);}));
    lefl_key_attach(&KEY_WHEEL, KEY_EVENT_DOWN, LAMBDA(void,(void*k){lefl_menu_click(&keylist);}));
    lefl_key_attach(&KEY_KNOB_CLOCKWISE, KEY_EVENT_DOWN, LAMBDA(void,(void*k){lefl_menu_index_increase(&keylist, 1);}));
    lefl_key_attach(&KEY_KNOB_ANTICLOCKWISE, KEY_EVENT_DOWN, LAMBDA(void,(void*k){lefl_menu_index_increase(&keylist, -1);}));
    lefl_key_attach(&KEY_WHEEL_CLOCKWISE, KEY_EVENT_DOWN, LAMBDA(void,(void*k){lefl_menu_index_increase(&keylist, 1);}));
    lefl_key_attach(&KEY_WHEEL_ANTICLOCKWISE, KEY_EVENT_DOWN, LAMBDA(void,(void*k){lefl_menu_index_increase(&keylist, -1);}));
}
