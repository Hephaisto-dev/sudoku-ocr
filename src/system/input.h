#pragma once

#include "common.h"

#include "system/window.h"


//#define _FROM_SDL_EV(ev) ((uint16_t)ev)


#define EVENT_KEY_PRESS      0 // _FROM_SDL_EV(SDL_KEYDOWN)
#define EVENT_KEY_RELEASE    1 // _FROM_SDL_EV(SDL_KEYUP)
#define EVENT_MOUSE_PRESS    2 // _FROM_SDL_EV(SDL_MOUSEBUTTONDOWN)
#define EVENT_MOUSE_RELEASE  3 // _FROM_SDL_EV(SDL_MOUSEBUTTONUP)
#define EVENT_MOUSE_MOVE     4 // _FROM_SDL_EV(SDL_MOUSEMOTION)
#define EVENT_MOUSE_SCROLL   5 //
#define EVENT_QUIT           6 // _FROM_SDL_EV(SDL_QUIT)
#define EVENT_TIMER          7 //
#define EVENT_WIN_RESIZED    8 // _FROM_SDL_EV(SDL_WINDOWEVENT_RESIZED)
#define EVENT_WIN_MOVED      9 // _FROM_SDL_EV(SDL_WINDOWEVENT_MOVED)
#define EVENT_WIN_ENTER     10 // _FROM_SDL_EV(SDL_WINDOWEVENT_ENTER)
#define EVENT_WIN_LEAVE     11 // _FROM_SDL_EV(SDL_WINDOWEVENT_LEAVE)
#define EVENT_WIN_FOCUSED   12 // _FROM_SDL_EV(SDL_WINDOWEVENT_FOCUS_GAINED)
#define EVENT_WIN_UNFOCUSED 13 // _FROM_SDL_EV(SDL_WINDOWEVENT_FOCUS_LOST)
#define EVENT_WIN_MINIMIZED 14 // _FROM_SDL_EV(SDL_WINDOWEVENT_MINIMIZED)
#define EVENT_WIN_MAXIMIZED 15 // _FROM_SDL_EV(SDL_WINDOWEVENT_MAXIMIZED)
#define EVENT_WIN_RESTORED  16 //
#define EVENT_WIN_PAINT     17 //
#define EVENT_WIN_CLOSE     18 // _FROM_SDL_EV(SDL_WINDOWEVENT_CLOSE)



typedef struct event_button_s {
    int32_t x;
    int32_t y;
    uint8_t button;
} event_button_t;


typedef struct event_motion_s {
    int32_t x;
    int32_t y;
} event_motion_t;


typedef struct event_scroll_s {
    float x;
    float y;
} event_scroll_t;


typedef struct event_keyboard_s {
    uint16_t keycode;
} event_keyboard_t;


typedef struct event_s {
    uint16_t type;
    uint32_t timestamp;
    uint32_t window;
    union {
        event_button_t button;
        event_motion_t motion;
        event_keyboard_t key;
        event_scroll_t scroll;
    };
} event_t;



void input_init();

void input_destroy();

void input_wait();

bool input_availables();

bool input_get(event_t* ev);

bool input_is_mouse_pressed();




// Keys

#define KEY_0                  SDLK_0
#define KEY_1                  SDLK_1
#define KEY_2                  SDLK_2
#define KEY_3                  SDLK_3
#define KEY_4                  SDLK_4
#define KEY_5                  SDLK_5
#define KEY_6                  SDLK_6
#define KEY_7                  SDLK_7
#define KEY_8                  SDLK_8
#define KEY_9                  SDLK_9
#define KEY_A                  SDLK_A
#define KEY_AC_BACK            SDLK_AC_BACK
#define KEY_AC_FORWARD         SDLK_AC_FORWARD
#define KEY_AC_HOME            SDLK_AC_HOME
#define KEY_AC_REFRESH         SDLK_AC_REFRESH
#define KEY_AC_SEARCH          SDLK_AC_SEARCH
#define KEY_AC_STOP            SDLK_AC_STOP
#define KEY_AGAIN              SDLK_AGAIN
#define KEY_ALTERASE           SDLK_ALTERASE
#define KEY_QUOTE              SDLK_QUOTE
#define KEY_APPLICATION        SDLK_APPLICATION
#define KEY_AUDIOMUTE          SDLK_AUDIOMUTE
#define KEY_AUDIONEXT          SDLK_AUDIONEXT
#define KEY_AUDIOPLAY          SDLK_AUDIOPLAY
#define KEY_AUDIOPREV          SDLK_AUDIOPREV
#define KEY_AUDIOSTOP          SDLK_AUDIOSTOP
#define KEY_B                  SDLK_B
#define KEY_BACKSLASH          SDLK_BACKSLASH
#define KEY_BACKSPACE          SDLK_BACKSPACE
#define KEY_BRIGHTNESSDOWN     SDLK_BRIGHTNESSDOWN
#define KEY_BRIGHTNESSUP       SDLK_BRIGHTNESSUP
#define KEY_C                  SDLK_C
#define KEY_CALCULATOR         SDLK_CALCULATOR
#define KEY_CANCEL             SDLK_CANCEL
#define KEY_CAPSLOCK           SDLK_CAPSLOCK
#define KEY_CLEAR              SDLK_CLEAR
#define KEY_CLEARAGAIN         SDLK_CLEARAGAIN
#define KEY_COMMA              SDLK_COMMA
#define KEY_COMPUTER           SDLK_COMPUTER
#define KEY_COPY               SDLK_COPY
#define KEY_CRSEL              SDLK_CRSEL
#define KEY_CURRENCYSUBUNIT    SDLK_CURRENCYSUBUNIT
#define KEY_CURRENCYUNIT       SDLK_CURRENCYUNIT
#define KEY_CUT                SDLK_CUT
#define KEY_D                  SDLK_D
#define KEY_DECIMALSEPARATOR   SDLK_DECIMALSEPARATOR
#define KEY_DELETE             SDLK_DELETE
#define KEY_DISPLAYSWITCH      SDLK_DISPLAYSWITCH
#define KEY_DOWN               SDLK_DOWN
#define KEY_E                  SDLK_E
#define KEY_EJECT              SDLK_EJECT
#define KEY_END                SDLK_END
#define KEY_EQUALS             SDLK_EQUALS
#define KEY_ESCAPE             SDLK_ESCAPE
#define KEY_EXECUTE            SDLK_EXECUTE
#define KEY_EXSEL              SDLK_EXSEL
#define KEY_F                  SDLK_F
#define KEY_F1                 SDLK_F1
#define KEY_F10                SDLK_F10
#define KEY_F11                SDLK_F11
#define KEY_F12                SDLK_F12
#define KEY_F13                SDLK_F13
#define KEY_F14                SDLK_F14
#define KEY_F15                SDLK_F15
#define KEY_F16                SDLK_F16
#define KEY_F17                SDLK_F17
#define KEY_F18                SDLK_F18
#define KEY_F19                SDLK_F19
#define KEY_F2                 SDLK_F2
#define KEY_F20                SDLK_F20
#define KEY_F21                SDLK_F21
#define KEY_F22                SDLK_F22
#define KEY_F23                SDLK_F23
#define KEY_F24                SDLK_F24
#define KEY_F3                 SDLK_F3
#define KEY_F4                 SDLK_F4
#define KEY_F5                 SDLK_F5
#define KEY_F6                 SDLK_F6
#define KEY_F7                 SDLK_F7
#define KEY_F8                 SDLK_F8
#define KEY_F9                 SDLK_F9
#define KEY_FIND               SDLK_FIND
#define KEY_G                  SDLK_G
#define KEY_BACKQUOTE          SDLK_BACKQUOTE
#define KEY_H                  SDLK_H
#define KEY_HELP               SDLK_HELP
#define KEY_HOME               SDLK_HOME
#define KEY_I                  SDLK_I
#define KEY_INSERT             SDLK_INSERT
#define KEY_J                  SDLK_J
#define KEY_K                  SDLK_K
#define KEY_KBDILLUMDOWN       SDLK_KBDILLUMDOWN
#define KEY_KBDILLUMTOGGLE     SDLK_KBDILLUMTOGGLE
#define KEY_KBDILLUMUP         SDLK_KBDILLUMUP
#define KEY_KP_0               SDLK_KP_0
#define KEY_KP_00              SDLK_KP_00
#define KEY_KP_000             SDLK_KP_000
#define KEY_KP_1               SDLK_KP_1
#define KEY_KP_2               SDLK_KP_2
#define KEY_KP_3               SDLK_KP_3
#define KEY_KP_4               SDLK_KP_4
#define KEY_KP_5               SDLK_KP_5
#define KEY_KP_6               SDLK_KP_6
#define KEY_KP_7               SDLK_KP_7
#define KEY_KP_8               SDLK_KP_8
#define KEY_KP_9               SDLK_KP_9
#define KEY_KP_A               SDLK_KP_A
#define KEY_KP_AMPERSAND       SDLK_KP_AMPERSAND
#define KEY_KP_AT              SDLK_KP_AT
#define KEY_KP_B               SDLK_KP_B
#define KEY_KP_BACKSPACE       SDLK_KP_BACKSPACE
#define KEY_KP_BINARY          SDLK_KP_BINARY
#define KEY_KP_C               SDLK_KP_C
#define KEY_KP_CLEAR           SDLK_KP_CLEAR
#define KEY_KP_CLEARENTRY      SDLK_KP_CLEARENTRY
#define KEY_KP_COLON           SDLK_KP_COLON
#define KEY_KP_COMMA           SDLK_KP_COMMA
#define KEY_KP_D               SDLK_KP_D
#define KEY_KP_DBLAMPERSAND    SDLK_KP_DBLAMPERSAND
#define KEY_KP_DBLVERTICALBAR  SDLK_KP_DBLVERTICALBAR
#define KEY_KP_DECIMAL         SDLK_KP_DECIMAL
#define KEY_KP_DIVIDE          SDLK_KP_DIVIDE
#define KEY_KP_E               SDLK_KP_E
#define KEY_KP_ENTER           SDLK_KP_ENTER
#define KEY_KP_EQUALS          SDLK_KP_EQUALS
#define KEY_KP_EQUALSAS400     SDLK_KP_EQUALSAS400
#define KEY_KP_EXCLAM          SDLK_KP_EXCLAM
#define KEY_KP_F               SDLK_KP_F
#define KEY_KP_GREATER         SDLK_KP_GREATER
#define KEY_KP_HASH            SDLK_KP_HASH
#define KEY_KP_HEXADECIMAL     SDLK_KP_HEXADECIMAL
#define KEY_KP_LEFTBRACE       SDLK_KP_LEFTBRACE
#define KEY_KP_LEFTPAREN       SDLK_KP_LEFTPAREN
#define KEY_KP_LESS            SDLK_KP_LESS
#define KEY_KP_MEMADD          SDLK_KP_MEMADD
#define KEY_KP_MEMCLEAR        SDLK_KP_MEMCLEAR
#define KEY_KP_MEMDIVIDE       SDLK_KP_MEMDIVIDE
#define KEY_KP_MEMMULTIPLY     SDLK_KP_MEMMULTIPLY
#define KEY_KP_MEMRECALL       SDLK_KP_MEMRECALL
#define KEY_KP_MEMSTORE        SDLK_KP_MEMSTORE
#define KEY_KP_MEMSUBTRACT     SDLK_KP_MEMSUBTRACT
#define KEY_KP_MINUS           SDLK_KP_MINUS
#define KEY_KP_MULTIPLY        SDLK_KP_MULTIPLY
#define KEY_KP_OCTAL           SDLK_KP_OCTAL
#define KEY_KP_PERCENT         SDLK_KP_PERCENT
#define KEY_KP_PERIOD          SDLK_KP_PERIOD
#define KEY_KP_PLUS            SDLK_KP_PLUS
#define KEY_KP_PLUSMINUS       SDLK_KP_PLUSMINUS
#define KEY_KP_POWER           SDLK_KP_POWER
#define KEY_KP_RIGHTBRACE      SDLK_KP_RIGHTBRACE
#define KEY_KP_RIGHTPAREN      SDLK_KP_RIGHTPAREN
#define KEY_KP_SPACE           SDLK_KP_SPACE
#define KEY_KP_TAB             SDLK_KP_TAB
#define KEY_KP_VERTICALBAR     SDLK_KP_VERTICALBAR
#define KEY_KP_XOR             SDLK_KP_XOR
#define KEY_L                  SDLK_L
#define KEY_LALT               SDLK_LALT
#define KEY_LCTRL              SDLK_LCTRL
#define KEY_LEFT               SDLK_LEFT
#define KEY_LEFTBRACKET        SDLK_LEFTBRACKET
#define KEY_LGUI               SDLK_LGUI
#define KEY_LSHIFT             SDLK_LSHIFT
#define KEY_M                  SDLK_M
#define KEY_MAIL               SDLK_MAIL
#define KEY_MEDIASELECT        SDLK_MEDIASELECT
#define KEY_MENU               SDLK_MENU
#define KEY_MINUS              SDLK_MINUS
#define KEY_MODE               SDLK_MODE
#define KEY_MUTE               SDLK_MUTE
#define KEY_N                  SDLK_N
#define KEY_NUMLOCKCLEAR       SDLK_NUMLOCKCLEAR
#define KEY_O                  SDLK_O
#define KEY_OPER               SDLK_OPER
#define KEY_OUT                SDLK_OUT
#define KEY_P                  SDLK_P
#define KEY_PAGEDOWN           SDLK_PAGEDOWN
#define KEY_PAGEUP             SDLK_PAGEUP
#define KEY_PASTE              SDLK_PASTE
#define KEY_PAUSE              SDLK_PAUSE
#define KEY_PERIOD             SDLK_PERIOD
#define KEY_POWER              SDLK_POWER
#define KEY_PRINTSCREEN        SDLK_PRINTSCREEN
#define KEY_PRIOR              SDLK_PRIOR
#define KEY_Q                  SDLK_Q
#define KEY_R                  SDLK_R
#define KEY_RALT               SDLK_RALT
#define KEY_RCTRL              SDLK_RCTRL
#define KEY_RETURN             SDLK_RETURN
#define KEY_RETURN2            SDLK_RETURN2
#define KEY_RGUI               SDLK_RGUI
#define KEY_RIGHT              SDLK_RIGHT
#define KEY_RIGHTBRACKET       SDLK_RIGHTBRACKET
#define KEY_RSHIFT             SDLK_RSHIFT
#define KEY_S                  SDLK_S
#define KEY_SCROLLLOCK         SDLK_SCROLLLOCK
#define KEY_SELECT             SDLK_SELECT
#define KEY_SEMICOLON          SDLK_SEMICOLON
#define KEY_SEPARATOR          SDLK_SEPARATOR
#define KEY_SLASH              SDLK_SLASH
#define KEY_SLEEP              SDLK_SLEEP
#define KEY_SPACE              SDLK_SPACE
#define KEY_STOP               SDLK_STOP
#define KEY_SYSREQ             SDLK_SYSREQ
#define KEY_T                  SDLK_T
#define KEY_TAB                SDLK_TAB
#define KEY_THOUSANDSSEPARATOR SDLK_THOUSANDSSEPARATOR
#define KEY_U                  SDLK_U
#define KEY_UNDO               SDLK_UNDO
#define KEY_UNKNOWN            SDLK_UNKNOWN
#define KEY_UP                 SDLK_UP
#define KEY_V                  SDLK_V
#define KEY_VOLUMEDOWN         SDLK_VOLUMEDOWN
#define KEY_VOLUMEUP           SDLK_VOLUMEUP
#define KEY_W                  SDLK_W
#define KEY_WWW                SDLK_WWW
#define KEY_X                  SDLK_X
#define KEY_Y                  SDLK_Y
#define KEY_ZSDLK_AMPERSAND    SDLK_ZSDLK_AMPERSAND
#define KEY_ASTERISK           SDLK_ASTERISK
#define KEY_AT                 SDLK_AT
#define KEY_CARET              SDLK_CARET
#define KEY_COLON              SDLK_COLON
#define KEY_DOLLAR             SDLK_DOLLAR
#define KEY_EXCLAIM            SDLK_EXCLAIM
#define KEY_GREATER            SDLK_GREATER
#define KEY_HASH               SDLK_HASH
#define KEY_LEFTPAREN          SDLK_LEFTPAREN
#define KEY_LESS               SDLK_LESS
#define KEY_PERCENT            SDLK_PERCENT
#define KEY_PLUS               SDLK_PLUS
#define KEY_QUESTION           SDLK_QUESTION
#define KEY_QUOTEDBL           SDLK_QUOTEDBL
#define KEY_RIGHTPAREN         SDLK_RIGHTPAREN
#define KEY_UNDERSCORE         SDLK_UNDERSCORE
