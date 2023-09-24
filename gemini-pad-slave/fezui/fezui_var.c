/*
 * fezui_var.c
 *
 *  Created on: 2023年6月19日
 *      Author: xq123
 */
#include "fezui_var.h"

char fpsstr[8] = {0};
char comstr[8] = {0};

fezui_t fezui;

bool eeprom_enable = true;
uint8_t key_triggered_count = 0;
uint8_t key_buffer[ADVANCED_KEY_NUM+KEY_NUM] = {0};
uint8_t cmd_buffer = 0;

lefl_advanced_key_t Keyboard_AdvancedKeys[ADVANCED_KEY_NUM];
lefl_key_t Keyboard_Keys[KEY_NUM];
uint16_t Keyboard_Advanced_SHIFT_IDs[ADVANCED_KEY_NUM];
uint16_t Keyboard_SHIFT_IDs[KEY_NUM];
uint16_t Keyboard_Advanced_ALPHA_IDs[ADVANCED_KEY_NUM];
uint16_t Keyboard_ALPHA_IDs[KEY_NUM];

lefl_bit_array_t Keyboard_Tree_ReportBitmap;
lefl_bit_array_unit_t Keyboard_Tree_ReportBuffer[KEYBOARD_TREE_BUFFER_MAX_SIZE/(8*sizeof(lefl_bit_array_unit_t))];
uint16_t Keyboard_Tree_IDs[3][KEYBOARD_TREE_BUFFER_MAX_SIZE];

uint32_t fezui_keytotalcounts[ADVANCED_KEY_NUM]={0};
uint32_t fezui_keyinitcounts[ADVANCED_KEY_NUM]={0};

lefl_loop_array_t KPS_history;
lefl_array_elm_t KPS_history_data[KPS_HISTORY_LENGTH];
lefl_loop_array_t KPS_queue;
lefl_array_elm_t KPS_queue_data[REFRESH_RATE];
uint8_t KPS_history_max=0;
uint32_t fezui_fps = 0;
uint8_t fezui_kps = 0;
uint8_t UI_KPSMaximumPerSecond = 0;
uint8_t fezui_debug;
uint8_t fezui_adc_conversion_count;
float fezui_debug_float;
uint32_t fezui_run_time;
uint32_t fezui_temp_raw;

uint8_t current_key_index;
uint8_t current_function_key;

/*
  Fontname: fez_font_6x10
  Copyright: Created with Fony 1.4.6
  Glyphs: 26/26
  BBX Build Mode: 0x
*/
const uint8_t fez_font_6x10_m[204] U8G2_FONT_SECTION("fez_font_6x10_m") =
  "\32\0\3\3\3\4\4\5\4\6\12\0\0\11\0\12\12\0\0\0\0\0\257 \5\0\324\16!\5\0\324"
  "\16\42\5\0\324\16#\5\0\324\16$\5\0\324\16%\5\0\324\16&\5\0\324\16'\5\0\324\16("
  "\5\0\324\16)\5\0\324\16*\5\0\324\16+\5\0\324\16,\5\0\324\16-\6\16\204\16\3.\5"
  "\0\324\16/\5\0\324\16\60\10M\204\16\233\276\31\61\7I\206\216\203\0\62\12M\204\216b\342\61\261"
  "\0\63\12M\204\216b\242\61\321\0\64\11M\204\216\230\66c\16\65\12M\204\16cb\61\321\0\66\12"
  "M\204\16c\242M\315\0\67\10M\204\216b\376\0\70\12M\204\16\233\332M\315\0\71\12M\204\16\233"
  "\232\61\321\0\0\0\0\4\377\377\0";


lefl_link_frame_t mainframe;

lefl_loop_array_t analog_historys[4];
lefl_array_elm_t analog_history1_data[HISTORY_LENGTH];
lefl_array_elm_t analog_history2_data[HISTORY_LENGTH];
lefl_array_elm_t analog_history3_data[HISTORY_LENGTH];
lefl_array_elm_t analog_history4_data[HISTORY_LENGTH];
fezui_cursor_t cursor={0,0,128,64};
fezui_cursor_t target_cursor={0,0,0,0};

lefl_bit_array_t lines[4];
lefl_bit_array_unit_t lines1_data[4];
lefl_bit_array_unit_t lines2_data[4];
lefl_bit_array_unit_t lines3_data[4];
lefl_bit_array_unit_t lines4_data[4];

const char *hid_usage_names[138] =
{
    "Left Control",
    "Left Shift",
    "Left Alt",
    "Left GUI(Win)",
    "Right Control",
    "Right Shift",
    "Right Alt",
    "Right GUI(Win)",
    "No Event",
    "Overrun Error",
    "POST Fail",
    "ErrorUndefined",
    "A",
    "B",
    "C",
    "D",
    "E",
    "F",
    "G",
    "H",
    "I",
    "J",
    "K",
    "L",
    "M",
    "N",
    "O",
    "P",
    "Q",
    "R",
    "S",
    "T",
    "U",
    "V",
    "W",
    "X",
    "Y",
    "Z",
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
    "Return",
    "Escape",
    "Backspace",
    "Tab",
    "Space",
    "-",
    "=",
    "[",
    "]",
    "\\",
    "Europe 1 (Note 2)",
    ";",
    "\"",
    "`",
    ",",
    ".",
    "/",
    "Caps Lock",
    "F1",
    "F2",
    "F3",
    "F4",
    "F5",
    "F6",
    "F7",
    "F8",
    "F9",
    "F10",
    "F11",
    "F12",
    "Print Screen",
    "Scroll Lock",
    "Break (Pause)",
    "Insert",
    "Home",
    "Page Up",
    "Delete",
    "End",
    "Page Down",
    "Right Arrow",
    "Left Arrow",
    "Down Arrow",
    "Up Arrow",
    "Num Lock",
    "Keypad /",
    "Keypad *",
    "Keypad -",
    "Keypad +",
    "Keypad Enter",
    "Keypad 1",
    "Keypad 2",
    "Keypad 3",
    "Keypad 4",
    "Keypad 5",
    "Keypad 6",
    "Keypad 7",
    "Keypad 8",
    "Keypad 9",
    "Keypad 0",
    "Keypad .",
    "Europe 2 (Note 2)",
    "App",
    "Keyboard Power",
    "Keypad =",
    "F13",
    "F14",
    "F15",
    "F16",
    "F17",
    "F18",
    "F19",
    "F20",
    "F21",
    "F22",
    "F23",
    "F24",
    "Execute",
    "Help",
    "Menu",
    "Select",
    "Stop",
    "Again",
    "Undo",
    "Cut",
    "Copy",
    "Paste",
    "Find",
    "Mute",
    "Volume Up",
    "Volume Down"
};



