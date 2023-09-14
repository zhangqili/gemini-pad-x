#include "fezui.h"
#include "fezui_var.h"

#define MARGIN_TILE 3

uint8_t selection;

bool Keyboard_SHIFT_Flag;
bool Keyboard_ALPHA_Flag;
void panelpage_init()
{

}

void panelpage_logic(void *page)
{
    fezui_cursor_set(
            &target_cursor ,
            (32*selection)%128,
            (selection)/4*32,
            32,
            32);
}

void panelpage_draw(void *page)
{
    uint8_t color = u8g2_GetDrawColor(&(fezui.u8g2));

    u8g2_SetFont(&(fezui.u8g2), u8g2_font_4x6_mr);

    if(Keyboard_AdvancedKeys[0].key.state)
    {
        u8g2_DrawBox(&(fezui.u8g2), 32*0+MARGIN_TILE, MARGIN_TILE, 32-MARGIN_TILE*2, 32-MARGIN_TILE*2);
    }
    else
    {
        u8g2_DrawFrame(&(fezui.u8g2), 32*0+MARGIN_TILE, MARGIN_TILE, 32-MARGIN_TILE*2, 32-MARGIN_TILE*2);
    }

    if(Keyboard_AdvancedKeys[1].key.state)
    {
        u8g2_DrawBox(&(fezui.u8g2), 32*1+MARGIN_TILE, MARGIN_TILE, 32-MARGIN_TILE*2, 32-MARGIN_TILE*2);
    }
    else
    {
        u8g2_DrawFrame(&(fezui.u8g2), 32*1+MARGIN_TILE, MARGIN_TILE, 32-MARGIN_TILE*2, 32-MARGIN_TILE*2);
    }

    if(Keyboard_AdvancedKeys[2].key.state)
    {
        u8g2_DrawBox(&(fezui.u8g2), 32*2+MARGIN_TILE, MARGIN_TILE, 32-MARGIN_TILE*2, 32-MARGIN_TILE*2);
    }
    else
    {
        u8g2_DrawFrame(&(fezui.u8g2), 32*2+MARGIN_TILE, MARGIN_TILE, 32-MARGIN_TILE*2, 32-MARGIN_TILE*2);
    }

    if(Keyboard_AdvancedKeys[3].key.state)
    {
        u8g2_DrawBox(&(fezui.u8g2), 32*3+MARGIN_TILE, MARGIN_TILE, 32-MARGIN_TILE*2, 32-MARGIN_TILE*2);
    }
    else
    {
        u8g2_DrawFrame(&(fezui.u8g2), 32*3+MARGIN_TILE, MARGIN_TILE, 32-MARGIN_TILE*2, 32-MARGIN_TILE*2);
    }

    u8g2_DrawFrame(&(fezui.u8g2), 32*0+MARGIN_TILE, 32+MARGIN_TILE, 32-MARGIN_TILE*2, 32-MARGIN_TILE*2);
    u8g2_DrawFrame(&(fezui.u8g2), 32*1+MARGIN_TILE, 32+MARGIN_TILE, 32-MARGIN_TILE*2, 32-MARGIN_TILE*2);
    if(Keyboard_SHIFT_Flag)
    {
        u8g2_DrawBox(&(fezui.u8g2), 32*2+MARGIN_TILE, 32+MARGIN_TILE, 32-MARGIN_TILE*2, 32-MARGIN_TILE*2);
    }
    else
    {
        u8g2_DrawFrame(&(fezui.u8g2), 32*2+MARGIN_TILE, 32+MARGIN_TILE, 32-MARGIN_TILE*2, 32-MARGIN_TILE*2);
    }
    if(Keyboard_ALPHA_Flag)
    {
        u8g2_DrawBox(&(fezui.u8g2), 32*3+MARGIN_TILE, 32+MARGIN_TILE, 32-MARGIN_TILE*2, 32-MARGIN_TILE*2);
    }
    else
    {
        u8g2_DrawFrame(&(fezui.u8g2), 32*3+MARGIN_TILE, 32+MARGIN_TILE, 32-MARGIN_TILE*2, 32-MARGIN_TILE*2);
    }

    if(Keyboard_Keys[2].state)
    {

        u8g2_DrawBox(&(fezui.u8g2), 32*1+MARGIN_TILE, 32+MARGIN_TILE, 32-MARGIN_TILE*2, 32-MARGIN_TILE*2);
    }

    if(Keyboard_Keys[3].state)
    {
        u8g2_DrawBox(&(fezui.u8g2), 32*0+MARGIN_TILE, 32+MARGIN_TILE, 32-MARGIN_TILE*2, 32-MARGIN_TILE*2);
    }

    if(Keyboard_Keys[4].state)
    {
        u8g2_DrawBox(&(fezui.u8g2), 32*0+MARGIN_TILE, 32+MARGIN_TILE, (32-MARGIN_TILE*2)/2, 32-MARGIN_TILE*2);
    }
    if(Keyboard_Keys[5].state)
    {
        u8g2_DrawBox(&(fezui.u8g2), 32*0+16+MARGIN_TILE, 32+MARGIN_TILE, (32-MARGIN_TILE*2)/2, 32-MARGIN_TILE*2);
    }

    if(Keyboard_Keys[6].state)
    {
        u8g2_DrawBox(&(fezui.u8g2), 32*1+MARGIN_TILE, 32+MARGIN_TILE, 32-MARGIN_TILE*2, (32-MARGIN_TILE*2)/2);
    }
    if(Keyboard_Keys[7].state)
    {
        u8g2_DrawBox(&(fezui.u8g2), 32*1+MARGIN_TILE, 32+16, 32-MARGIN_TILE*2, (32-MARGIN_TILE*2)/2);
    }
    u8g2_SetDrawColor(&(fezui.u8g2), 2);

    u8g2_DrawStr(&(fezui.u8g2), 32*0+16-8,16,"KEY1");
    u8g2_DrawStr(&(fezui.u8g2), 32*1+16-8,16,"KEY2");
    u8g2_DrawStr(&(fezui.u8g2), 32*2+16-8,16,"KEY3");
    u8g2_DrawStr(&(fezui.u8g2), 32*3+16-8,16,"KEY4");
    u8g2_DrawStr(&(fezui.u8g2), 32*0+16-10,32+16,"WHEEL");
    u8g2_DrawStr(&(fezui.u8g2), 32*1+16-8,32+16,"KNOB");
    u8g2_DrawStr(&(fezui.u8g2), 32*2+16-10,32+16,"SHIFT");
    u8g2_DrawStr(&(fezui.u8g2), 32*3+16-10,32+16,"ALPHA");

    u8g2_SetDrawColor(&(fezui.u8g2), color);
    fezui_draw_cursor(&fezui, &cursor);
}

static void selection_update(int8_t x)
{
    if(selection+x<0)
    {
        selection+=8+x;
    }
    else if(selection+x>=8)
    {
        selection+=x-8;
    }
    else
    {
        selection+=x;
    }
    if(selection<0)
    {
        selection=7;
    }
    if(selection>=8)
    {
        selection=0;
    }
}

static void wheel_cb(void *k)
{
    if(selection==6)
    {
        Keyboard_SHIFT_Flag=!Keyboard_SHIFT_Flag;
        if(Keyboard_ALPHA_Flag)
            Keyboard_ALPHA_Flag=false;
    }
    if(selection==7)
    {
        Keyboard_ALPHA_Flag=!Keyboard_ALPHA_Flag;
        if(Keyboard_SHIFT_Flag)
            Keyboard_SHIFT_Flag=false;
    }
    if(Keyboard_SHIFT_Flag)
    {
        current_function_key = 1;
        if (selection<4)
        {
            current_target_id = &Keyboard_Advanced_SHIFT_IDs[selection];
            current_key_index = selection;
            sprintf(current_key_name,"KEY%d S",selection+1);
            lefl_link_frame_navigate(&mainframe,&keyconfigpage);
        }
        else if(selection==4)
        {
            current_target_knob_press_id = &Keyboard_SHIFT_IDs[KEY_WHEEL_INDEX];
            current_target_knob_clockwise_id = &Keyboard_SHIFT_IDs[KEY_WHEEL_CLOCKWISE_INDEX];
            current_target_knob_anticlockwise_id = &Keyboard_SHIFT_IDs[KEY_WHEEL_ANTICLOCKWISE_INDEX];
            sprintf(current_knob_name,"WHEEL S");
            lefl_link_frame_navigate(&mainframe,&knobconfigpage);
        }
        else if(selection==5)
        {
            current_target_knob_press_id = &Keyboard_SHIFT_IDs[KEY_KNOB_INDEX];
            current_target_knob_clockwise_id = &Keyboard_SHIFT_IDs[KEY_KNOB_CLOCKWISE_INDEX];
            current_target_knob_anticlockwise_id = &Keyboard_SHIFT_IDs[KEY_KNOB_ANTICLOCKWISE_INDEX];
            sprintf(current_knob_name,"KNOB S");
            lefl_link_frame_navigate(&mainframe,&knobconfigpage);
        }
        return;
    }
    if(Keyboard_ALPHA_Flag)
    {
        current_function_key = 2;
        if (selection<4)
        {
            current_target_id = &Keyboard_Advanced_ALPHA_IDs[selection];
            current_key_index = selection;
            sprintf(current_key_name,"KEY%d A",selection+1);
            lefl_link_frame_navigate(&mainframe,&keyconfigpage);
        }
        else if(selection==4)
        {
            current_target_knob_press_id = &Keyboard_ALPHA_IDs[KEY_WHEEL_INDEX];
            current_target_knob_clockwise_id = &Keyboard_ALPHA_IDs[KEY_WHEEL_CLOCKWISE_INDEX];
            current_target_knob_anticlockwise_id = &Keyboard_ALPHA_IDs[KEY_WHEEL_ANTICLOCKWISE_INDEX];
            sprintf(current_knob_name,"WHEEL A");
            lefl_link_frame_navigate(&mainframe,&knobconfigpage);
        }
        else if(selection==5)
        {
            current_target_knob_press_id = &Keyboard_ALPHA_IDs[KEY_KNOB_INDEX];
            current_target_knob_clockwise_id = &Keyboard_ALPHA_IDs[KEY_KNOB_CLOCKWISE_INDEX];
            current_target_knob_anticlockwise_id = &Keyboard_ALPHA_IDs[KEY_KNOB_ANTICLOCKWISE_INDEX];
            sprintf(current_knob_name,"KNOB A");
            lefl_link_frame_navigate(&mainframe,&knobconfigpage);
        }
        return;
    }
    current_function_key = 0;
    if (selection<4)
    {
        current_config_advanced_key = Keyboard_AdvancedKeys + selection;
        current_key_index = selection;
        lefl_link_frame_navigate(&mainframe,&advancedconfigpage);
    }
    else if(selection==4)
    {
        current_target_knob_press_id = &KEY_WHEEL.id;
        current_target_knob_clockwise_id = &KEY_WHEEL_CLOCKWISE.id;
        current_target_knob_anticlockwise_id = &KEY_WHEEL_ANTICLOCKWISE.id;
        sprintf(current_knob_name,"WHEEL");
        lefl_link_frame_navigate(&mainframe,&knobconfigpage);
    }
    else if(selection==5)
    {
        current_target_knob_press_id = &KEY_KNOB.id;
        current_target_knob_clockwise_id =  &KEY_KNOB_CLOCKWISE.id;
        current_target_knob_anticlockwise_id = &KEY_KNOB_ANTICLOCKWISE.id;
        sprintf(current_knob_name,"KNOB");
        lefl_link_frame_navigate(&mainframe,&knobconfigpage);
    }
}

void panelpage_load(void *page)
{
    lefl_key_attach(&KEY_KNOB, KEY_EVENT_DOWN, LAMBDA(void,(void*k){lefl_link_frame_go_back(&mainframe);}));
    lefl_key_attach(&KEY_WHEEL, KEY_EVENT_DOWN, wheel_cb);
    lefl_key_attach(&KEY_KNOB_CLOCKWISE, KEY_EVENT_DOWN, LAMBDA(void,(void*k){selection_update(1);}));
    lefl_key_attach(&KEY_KNOB_ANTICLOCKWISE, KEY_EVENT_DOWN, LAMBDA(void,(void*k){selection_update(-1);}));
    lefl_key_attach(&KEY_WHEEL_CLOCKWISE, KEY_EVENT_DOWN, LAMBDA(void,(void*k){selection_update(4);}));
    lefl_key_attach(&KEY_WHEEL_ANTICLOCKWISE, KEY_EVENT_DOWN, LAMBDA(void,(void*k){selection_update(-4);}));
}

lefl_page_t panelpage={panelpage_logic,panelpage_draw,panelpage_load};
