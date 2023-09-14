/*
 * fezui_keyselectpage.c
 *
 *  Created on: 2023年6月19日
 *      Author: xq123
 */
#include "fezui.h"
#include "fezui_var.h"
#include "usb_hid_keys.h"

static float Cursor_X = 0;
static float Cursor_TargetX = 0;
static float target_abscissa = 0;
// width: 5, height: 5
const unsigned char win_icon[] U8X8_PROGMEM = { 0x1b, 0x1b, 0x00, 0x1b, 0x1b };


const enum USB_HID_KEYBOARD_SCAN_CODES ansi_104_keycode[6][17] = {
        {KEY_ESC,KEY_F1,KEY_F2,KEY_F3,KEY_F4,KEY_F5,KEY_F6,KEY_F7,KEY_F8,KEY_F9,KEY_F10,KEY_F11,KEY_F12,KEY_PRINT_SCREEN,KEY_SCROLL_LOCK,KEY_PAUSE},
        {KEY_GRAVE,KEY_1,KEY_2,KEY_3,KEY_4,KEY_5,KEY_6,KEY_7,KEY_8,KEY_9,KEY_0,KEY_MINUS,KEY_EQUAL,KEY_BACKSPACE,KEY_INSERT,KEY_HOME,KEY_PAGE_UP},
        {KEY_TAB,KEY_Q,KEY_W,KEY_E,KEY_R,KEY_T,KEY_Y,KEY_U,KEY_I,KEY_O,KEY_P,KEY_LEFT_BRACE,KEY_RIGHT_BRACE,KEY_BACKSLASH,KEY_DELETE,KEY_END,KEY_PAGE_DOWN},
        {KEY_CAPS_LOCK,KEY_A,KEY_S,KEY_D,KEY_F,KEY_G,KEY_H,KEY_J,KEY_K,KEY_L,KEY_SEMICOLON,KEY_APOSTROPHE,KEY_ENTER},
        {KEY_LEFT_SHIFT,KEY_Z,KEY_X,KEY_C,KEY_V,KEY_B,KEY_N,KEY_M,KEY_COMMA,KEY_DOT,KEY_SLASH,KEY_RIGHT_SHIFT,KEY_NO_EVENT,KEY_UP_ARROW,KEY_NO_EVENT},
        {KEY_LEFT_CONTROL,KEY_LEFT_GUI,KEY_LEFT_ALT,KEY_SPACEBAR,KEY_RIGHT_ALT,KEY_RIGHT_GUI,KEY_MENU,KEY_RIGHT_CONTROL,KEY_LEFT_ARROW,KEY_DOWN_ARROW,KEY_RIGHT_ARROW}
};

const char *custom_hid_usage_id_name[] =
{
    "",
    "Overrun Error",
    "POST Fail",
    "ErrorUndefined",
    "A ",
    "B ",
    "C ",
    "D ",
    "E ",
    "F ",
    "G ",
    "H ",
    "I ",
    "J ",
    "K ",
    "L ",
    "M ",
    "N ",
    "O ",
    "P ",
    "Q ",
    "R ",
    "S ",
    "T ",
    "U ",
    "V ",
    "W ",
    "X ",
    "Y ",
    "Z ",
    "1 ",
    "2 ",
    "3 ",
    "4 ",
    "5 ",
    "6 ",
    "7 ",
    "8 ",
    "9 ",
    "0 ",
    "Return",
    "Esc      ",
    "Backspace  ",
    "Tab    ",
    "        Space        ",
    "- ",
    "= ",
    "[ ",
    "] ",
    "\\",
    "Europe 1",
    "; ",
    "' ",
    "` ",
    ", ",
    ". ",
    "/ ",
    "CapsLk  ",
    "F1",
    "F2",
    "F3",
    "F4  ",
    "F5",
    "F6",
    "F7",
    "F8  ",
    "F9",
    "F10",
    "F11",
    "F12  ",
    "Prtsc ",
    "Scroll Lock",
    "Pause Break",
    "Insert",
    "    Home   ",
    "       PgUp",
    "Delete",
    "    End    ",
    "       PgDn",
    "      Right",
    "Left  ",
    "    Down   ",
    "    Up     ",
    "Num Lock",
    "/",
    "*",
    "-",
    "+",
    "Enter",
    "1",
    "2",
    "3",
    "4",
    "5",
    "6",
    "7",
    "8",
    "9",
    "0",
    ".",
    "Europe 2",
    "App"
    "Keyboard Power",
    "Keypad =",
    "F13 ",
    "F14 ",
    "F15 ",
    "F16 ",
    "F17 ",
    "F18 ",
    "F19 ",
    "F20 ",
    "F21 ",
    "F22 ",
    "F23 ",
    "F24 ",
    "Keyboard Execute",
    "Keyboard Help",
    "Keyboard Menu",
    "Keyboard Select",
    "Keyboard Stop",
    "Keyboard Again",
    "Keyboard Undo",
    "Keyboard Cut",
    "Keyboard Copy",
    "Keyboard Paste",
    "Keyboard Find",
    "Keyboard Mute",
    "Keyboard Volume Up",
    "Keyboard Volume Dn",
    "Keyboard Locking Caps Lock",
    "Keyboard Locking Num Lock",
    "Keyboard Locking Scroll Lock"
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "Ctrl",
    "Shift    ",
    "Alt",
    "   ",
    "Ctrl  ",
    "       Shift  ",
    "Alt",
    "   ",
};


lefl_keyboard_t keyboard = { .x = 0, .y = 0 };

static fezui_scrollview_t scrollview = { .content_height = 64, .content_width =
        433, .abscissa = 0, .ordinate = 0 };

lefl_page_t keyselectpage = { keyselectpage_logic, keyselectpage_draw,
        keyselectpage_load };
void keyselectpage_logic(void *page)
{
    Cursor_TargetX = 0;
    for (uint8_t i = 0; i < keyboard.x; i++)
    {
        Cursor_TargetX += strlen(custom_hid_usage_id_name[ansi_104_keycode[keyboard.y][i]]) * 4 + 4;
    }
    target_abscissa = Cursor_TargetX
            + strlen(custom_hid_usage_id_name[ansi_104_keycode[keyboard.y][keyboard.x]]) * 4 / 2 - 64;
    if (target_abscissa >= 0)
    {
        fezui_cursor_set(&target_cursor,
                64 - strlen(custom_hid_usage_id_name[ansi_104_keycode[keyboard.y][keyboard.x]]) * 4 / 2,
                keyboard.y * 10,
                strlen(custom_hid_usage_id_name[ansi_104_keycode[keyboard.y][keyboard.x]]) * 4 + 1, 7);
    }
    else
    {
        target_abscissa = 0;
        fezui_cursor_set(&target_cursor, Cursor_TargetX, keyboard.y * 10,
                strlen(custom_hid_usage_id_name[ansi_104_keycode[keyboard.y][keyboard.x]]) * 4 + 1, 7);
    }
    CONVERGE_TO_ROUNDED(scrollview.abscissa, target_abscissa, fezui.speed);
}
void keyselectpage_draw(void *page)
{
    uint16_t delta_x = 0;
    u8g2_SetFont(&(fezui.u8g2), u8g2_font_micro_mr);

    for (uint8_t i = 0; i < 6; i++)
    {
        delta_x = 1;
        for (uint8_t j = 0; j < 17; j++)
        {
            u8g2_DrawStr(&(fezui.u8g2),
                    delta_x - (u8g2_int_t)scrollview.abscissa, 10 * (i + 1) - 4,
                    custom_hid_usage_id_name[ansi_104_keycode[i][j]]);
            delta_x += strlen(custom_hid_usage_id_name[ansi_104_keycode[i][j]]) * 4 + 4;
            if (delta_x > WIDTH + target_abscissa)
                break;
        }
        u8g2_DrawXBMP(&(fezui.u8g2),
                4 * 5 + 1 - (u8g2_int_t)scrollview.abscissa, 10 * 6 - 5 - 4, 5,
                5, win_icon);
        u8g2_DrawXBMP(&(fezui.u8g2),
                4 * 40 - 3 - (u8g2_int_t)scrollview.abscissa, 10 * 6 - 5 - 4, 5,
                5, win_icon);
    }
    sprintf(fezui_buffer,"%d",delta_x);
    u8g2_DrawStr(&(fezui.u8g2), 96, 48, fezui_buffer);

    fezui_draw_cursor(&fezui, &cursor);
    fezui_draw_scrollview(&fezui, 0, 0, 128, 64, &scrollview);
}

void keyselectpage_load(void *page)
{
    Cursor_X = 0;
    scrollview.abscissa = 0;
    lefl_key_attach(&KEY_KNOB, KEY_EVENT_DOWN, LAMBDA(void, (void*k){lefl_link_frame_go_back(&mainframe);}));
    lefl_key_attach(&KEY_WHEEL, KEY_EVENT_DOWN, LAMBDA(void,
            (void*k){fezui_cursor_set(&cursor ,cursor.x-3 ,cursor.y-3 ,cursor.w+6 ,cursor.h+6);}));
    lefl_key_attach(&KEY_KNOB_CLOCKWISE, KEY_EVENT_DOWN, LAMBDA(void, (void*k){lefl_keyboard_x_increase(&keyboard, 1);}));
    lefl_key_attach(&KEY_KNOB_ANTICLOCKWISE, KEY_EVENT_DOWN, LAMBDA(void, (void*k){lefl_keyboard_x_increase(&keyboard, -1);}));
    lefl_key_attach(&KEY_WHEEL_CLOCKWISE, KEY_EVENT_DOWN, LAMBDA(void, (void*k){lefl_keyboard_y_increase(&keyboard, 1);}));
    lefl_key_attach(&KEY_WHEEL_ANTICLOCKWISE, KEY_EVENT_DOWN, LAMBDA(void, (void*k){lefl_keyboard_y_increase(&keyboard, -1);}));
}
