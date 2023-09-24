/*
 * fezui_externalkeyspage.c
 *
 *  Created on: Sep 24, 2023
 *      Author: xq123
 */
#include "fezui.h"
#include "fezui_var.h"
#include "communication.h"
#include "main.h"
#include "tim.h"
lefl_menu_t externalkeysmenu;
lefl_page_t externalkeyspage={externalkeyspage_logic,externalkeyspage_draw,externalkeyspage_load};

#define ROW_HEIGHT 8
#define SUBROW_HEIGHT 16


#define SPERATOR_X (WIDTH-80)

static fezui_scrollview_t scrollview={.content_height=HEIGHT};
static uint8_t selected_key_index;
static float target_ordinate=0;
static float start_animation=0;

static char binding_text[128];
static char shift_binding_text[128];
static char alpha_binding_text[128];

static fezui_scrolling_text_t scrolling_text;
static fezui_scrolling_text_t shift_scrolling_text;
static fezui_scrolling_text_t alpha_scrolling_text;

static const char* externalkeys_menu_items[] = {"Normal", "SHIFT", "ALPHA"};
lefl_menu_t externalkeys_menu;

static bool key_selected=false;

static fezui_cursor_t config_cursor;
static fezui_cursor_t target_config_cursor;

void externalkeys_menu_cb(void *m)
{
    current_target_id=&(Keyboard_Tree_IDs[((lefl_menu_t*)m)->selected_index][selected_key_index]);
    lefl_link_frame_navigate(&mainframe, &keylistpage);
}

void externalkeyspage_init()
{
    lefl_menu_init(&externalkeys_menu, externalkeys_menu_items, sizeof(externalkeys_menu_items)/sizeof(const char*), externalkeys_menu_cb);
}

void externalkeyspage_logic(void *page)
{
    if(key_selected)
    {
        fezui_cursor_set(
                &target_config_cursor,
                SPERATOR_X,
                externalkeys_menu.selected_index*SUBROW_HEIGHT,
                WIDTH-SPERATOR_X,
                SUBROW_HEIGHT);
    }
    else
    {
        fezui_cursor_set(
                &target_config_cursor,
                SPERATOR_X,
                externalkeys_menu.selected_index*SUBROW_HEIGHT,
                1,
                SUBROW_HEIGHT);
    }
    fezui_cursor_set(
        &target_cursor ,
        0,
        selected_key_index*ROW_HEIGHT - (u8g2_int_t)scrollview.ordinate,
        SPERATOR_X-7,
        ROW_HEIGHT);
    if((selected_key_index+1)*ROW_HEIGHT-target_ordinate>64)
    {
        target_ordinate = (selected_key_index+1)*ROW_HEIGHT-64;
    }
    if((selected_key_index)*ROW_HEIGHT<target_ordinate)
    {
        target_ordinate = (selected_key_index)*ROW_HEIGHT;
    }
    CONVERGE_TO_ROUNDED(scrollview.ordinate, target_ordinate, fezui.speed);
    CONVERGE_TO(start_animation, 1, fezui.speed);
    fezui_cursor_move(&fezui, &config_cursor, &target_config_cursor);
    fezui_scrolling_text_update(&scrolling_text);
    fezui_scrolling_text_update(&shift_scrolling_text);
    fezui_scrolling_text_update(&alpha_scrolling_text);

}
void externalkeyspage_draw(void *page)
{
    u8g2_SetFont(&(fezui.u8g2), u8g2_font_5x8_mr);
    for(uint8_t i=0;i<Keyboard_Tree_ReportBitmap.len;i++)
    {
        sprintf(fezui_buffer,"ExKEY%d",i+1);
        u8g2_DrawStr(&(fezui.u8g2),1,ROUND((ROW_HEIGHT*(i+1) - 1 - scrollview.ordinate)*start_animation),fezui_buffer);
    }
    for(uint8_t i=0;i<externalkeys_menu.len;i++)
    {
        u8g2_DrawStr(&(fezui.u8g2),SPERATOR_X+2,SUBROW_HEIGHT*(i+1)-9,externalkeys_menu.items[i]);
    }
    fezui_draw_scrolling_text(&fezui, SPERATOR_X+2, SUBROW_HEIGHT*1-2, &scrolling_text);
    fezui_draw_scrolling_text(&fezui, SPERATOR_X+2, SUBROW_HEIGHT*2-2, &shift_scrolling_text);
    fezui_draw_scrolling_text(&fezui, SPERATOR_X+2, SUBROW_HEIGHT*3-2, &alpha_scrolling_text);

    fezui_draw_scrollview(&fezui, 0, 0, SPERATOR_X - 1, ROUND(HEIGHT*start_animation), &scrollview);
    fezui_draw_cursor(&fezui, &cursor);
    fezui_draw_cursor(&fezui, &config_cursor);
    u8g2_DrawVLine(&(fezui.u8g2), SPERATOR_X, 0, 64);
}

void externalkeysmenu_up_cb(void *page)
{
    if(key_selected)
    {
        lefl_menu_index_increase(&externalkeys_menu, -1);
    }
    else
    {
        VAR_LOOP_DECREMENT(selected_key_index,0,Keyboard_Tree_ReportBitmap.len,1);
    }
    keyid_prase(Keyboard_Tree_IDs[0][selected_key_index], binding_text, 128);
    fezui_scrolling_text_init(&scrolling_text, 78, 0.2, u8g2_font_4x6_mr, binding_text);

    keyid_prase(Keyboard_Tree_IDs[1][selected_key_index], shift_binding_text, 128);
    fezui_scrolling_text_init(&shift_scrolling_text, 78, 0.2, u8g2_font_4x6_mr, shift_binding_text);

    keyid_prase(Keyboard_Tree_IDs[2][selected_key_index], alpha_binding_text, 128);
    fezui_scrolling_text_init(&alpha_scrolling_text, 78, 0.2, u8g2_font_4x6_mr, alpha_binding_text);

}

void externalkeysmenu_down_cb(void *page)
{
    if(key_selected)
    {
        lefl_menu_index_increase(&externalkeys_menu, 1);
    }
    else
    {
        VAR_LOOP_INCREMENT(selected_key_index,0,Keyboard_Tree_ReportBitmap.len,1);
    }
    keyid_prase(Keyboard_Tree_IDs[0][selected_key_index], binding_text, 128);
    fezui_scrolling_text_init(&scrolling_text, 78, 0.2, u8g2_font_4x6_mr, binding_text);

    keyid_prase(Keyboard_Tree_IDs[1][selected_key_index], shift_binding_text, 128);
    fezui_scrolling_text_init(&shift_scrolling_text, 78, 0.2, u8g2_font_4x6_mr, shift_binding_text);

    keyid_prase(Keyboard_Tree_IDs[2][selected_key_index], alpha_binding_text, 128);
    fezui_scrolling_text_init(&alpha_scrolling_text, 78, 0.2, u8g2_font_4x6_mr, alpha_binding_text);

}

void externalkeyspage_load(void *page)
{
    fezui_cursor_set(
            &config_cursor,
            SPERATOR_X,
            0,
            1,
            HEIGHT);
    start_animation=0;
    keyid_prase(Keyboard_Tree_IDs[0][selected_key_index], binding_text, 128);
    fezui_scrolling_text_init(&scrolling_text, 78, 0.2, u8g2_font_4x6_mr, binding_text);

    keyid_prase(Keyboard_Tree_IDs[1][selected_key_index], shift_binding_text, 128);
    fezui_scrolling_text_init(&shift_scrolling_text, 78, 0.2, u8g2_font_4x6_mr, shift_binding_text);

    keyid_prase(Keyboard_Tree_IDs[2][selected_key_index], alpha_binding_text, 128);
    fezui_scrolling_text_init(&alpha_scrolling_text, 78, 0.2, u8g2_font_4x6_mr, alpha_binding_text);

    scrollview.content_height=Keyboard_Tree_ReportBitmap.len*ROW_HEIGHT;
    lefl_key_attach(&KEY_KNOB, KEY_EVENT_DOWN, LAMBDA(void,(void*k)
    {
        if(key_selected)
        {
            key_selected=false;
        }
        else
        {
            lefl_link_frame_go_back(&mainframe);
        }

    }));
    lefl_key_attach(&KEY_WHEEL, KEY_EVENT_DOWN, LAMBDA(void,(void*k)
    {
        if(key_selected)
        {
            lefl_menu_click(&externalkeys_menu);
        }
        else
        {
            key_selected=true;
        }
    }));
    lefl_key_attach(&KEY_KNOB_CLOCKWISE, KEY_EVENT_DOWN, LAMBDA(void,(void*k){externalkeysmenu_down_cb(NULL);}));
    lefl_key_attach(&KEY_KNOB_ANTICLOCKWISE, KEY_EVENT_DOWN, LAMBDA(void,(void*k){externalkeysmenu_up_cb(NULL);}));
    lefl_key_attach(&KEY_WHEEL_CLOCKWISE, KEY_EVENT_DOWN, LAMBDA(void,(void*k){externalkeysmenu_down_cb(NULL);}));
    lefl_key_attach(&KEY_WHEEL_ANTICLOCKWISE, KEY_EVENT_DOWN, LAMBDA(void,(void*k){externalkeysmenu_up_cb(NULL);}));
}


