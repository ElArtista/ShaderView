#include "input.h"
#include <string.h>
#include <windows.h>

void populate_keycode_map(int* map, unsigned int size)
{
    /* Set every mapping to invalid */
    memset(map, KEY_UNKNOWN, size);

    /* Populate windows scan map */
    map['0'] = KEY_NUM0;
    map['1'] = KEY_NUM1;
    map['2'] = KEY_NUM2;
    map['3'] = KEY_NUM3;
    map['4'] = KEY_NUM4;
    map['5'] = KEY_NUM5;
    map['6'] = KEY_NUM6;
    map['7'] = KEY_NUM7;
    map['8'] = KEY_NUM8;
    map['9'] = KEY_NUM9;

    map['A'] = KEY_A;
    map['B'] = KEY_B;
    map['C'] = KEY_C;
    map['D'] = KEY_D;
    map['E'] = KEY_E;
    map['F'] = KEY_F;
    map['G'] = KEY_G;
    map['H'] = KEY_H;
    map['I'] = KEY_I;
    map['J'] = KEY_J;
    map['K'] = KEY_K;
    map['L'] = KEY_L;
    map['M'] = KEY_M;
    map['N'] = KEY_N;
    map['O'] = KEY_O;
    map['P'] = KEY_P;
    map['Q'] = KEY_Q;
    map['R'] = KEY_R;
    map['S'] = KEY_S;
    map['T'] = KEY_T;
    map['U'] = KEY_U;
    map['V'] = KEY_V;
    map['W'] = KEY_W;
    map['X'] = KEY_X;
    map['Y'] = KEY_Y;
    map['Z'] = KEY_Z;

    map[VK_NUMPAD0] = KEY_KP0;
    map[VK_NUMPAD1] = KEY_KP1;
    map[VK_NUMPAD2] = KEY_KP2;
    map[VK_NUMPAD3] = KEY_KP3;
    map[VK_NUMPAD4] = KEY_KP4;
    map[VK_NUMPAD5] = KEY_KP5;
    map[VK_NUMPAD6] = KEY_KP6;
    map[VK_NUMPAD7] = KEY_KP7;
    map[VK_NUMPAD8] = KEY_KP8;
    map[VK_NUMPAD9] = KEY_KP9;

    map[VK_F1] = KEY_F1;
    map[VK_F2] = KEY_F2;
    map[VK_F3] = KEY_F3;
    map[VK_F4] = KEY_F4;
    map[VK_F5] = KEY_F5;
    map[VK_F6] = KEY_F6;
    map[VK_F7] = KEY_F7;
    map[VK_F8] = KEY_F8;
    map[VK_F9] = KEY_F9;
    map[VK_F10] = KEY_F10;
    map[VK_F11] = KEY_F11;
    map[VK_F12] = KEY_F12;
    map[VK_F13] = KEY_F13;
    map[VK_F14] = KEY_F14;
    map[VK_F15] = KEY_F15;
    map[VK_F16] = KEY_F16;
    map[VK_F17] = KEY_F17;
    map[VK_F18] = KEY_F18;
    map[VK_F19] = KEY_F19;
    map[VK_F20] = KEY_F20;
    map[VK_F21] = KEY_F21;
    map[VK_F22] = KEY_F22;
    map[VK_F23] = KEY_F23;
    map[VK_F24] = KEY_F24;

    map[0x028] = KEY_APOSTROPHE;
    map[0x02B] = KEY_BACKSLASH;
    map[0x033] = KEY_COMMA;
    map[0x00D] = KEY_EQUAL;
    map[0x029] = KEY_GRAVE_ACCENT;
    map[0x01A] = KEY_LEFT_BRACKET;
    map[0x00C] = KEY_MINUS;
    map[0x034] = KEY_PERIOD;
    map[0x01B] = KEY_RIGHT_BRACKET;
    map[0x027] = KEY_SEMICOLON;
    map[0x035] = KEY_SLASH;

    map[0x00E] = KEY_BACKSPACE;
    map[0x153] = KEY_DELETE;
    map[0x14F] = KEY_END;
    map[0x01C] = KEY_ENTER;
    map[0x001] = KEY_ESCAPE;
    map[0x147] = KEY_HOME;
    map[0x152] = KEY_INSERT;
    map[0x15D] = KEY_MENU;
    map[0x151] = KEY_PAGE_DOWN;
    map[0x149] = KEY_PAGE_UP;
    map[0x045] = KEY_PAUSE;
    map[0x039] = KEY_SPACE;
    map[0x00F] = KEY_TAB;
    map[0x03A] = KEY_CAPS_LOCK;
    map[0x145] = KEY_NUM_LOCK;
    map[0x046] = KEY_SCROLL_LOCK;

    map[0x038] = KEY_LEFT_ALT;
    map[0x01D] = KEY_LEFT_CONTROL;
    map[0x02A] = KEY_LEFT_SHIFT;
    map[0x15B] = KEY_LEFT_SUPER;
    map[0x137] = KEY_PRINT_SCREEN;
    map[0x138] = KEY_RIGHT_ALT;
    map[0x11D] = KEY_RIGHT_CONTROL;
    map[0x036] = KEY_RIGHT_SHIFT;
    map[0x15C] = KEY_RIGHT_SUPER;
    map[0x150] = KEY_DOWN;
    map[0x14B] = KEY_LEFT;
    map[0x14D] = KEY_RIGHT;
    map[0x148] = KEY_UP;

    map[0x04E] = KEY_KP_ADD;
    map[0x053] = KEY_KP_DECIMAL;
    map[0x135] = KEY_KP_DIVIDE;
    map[0x11C] = KEY_KP_ENTER;
    map[0x037] = KEY_KP_MULTIPLY;
    map[0x04A] = KEY_KP_SUBTRACT;
}
