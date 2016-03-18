/*********************************************************************************************************************/
/*                                                  /===-_---~~~~~~~~~------____                                     */
/*                                                 |===-~___                _,-'                                     */
/*                  -==\\                         `//~\\   ~~~~`---.___.-~~                                          */
/*              ______-==|                         | |  \\           _-~`                                            */
/*        __--~~~  ,-/-==\\                        | |   `\        ,'                                                */
/*     _-~       /'    |  \\                      / /      \      /                                                  */
/*   .'        /       |   \\                   /' /        \   /'                                                   */
/*  /  ____  /         |    \`\.__/-~~ ~ \ _ _/'  /          \/'                                                     */
/* /-'~    ~~~~~---__  |     ~-/~         ( )   /'        _--~`                                                      */
/*                   \_|      /        _)   ;  ),   __--~~                                                           */
/*                     '~~--_/      _-~/-  / \   '-~ \                                                               */
/*                    {\__--_/}    / \\_>- )<__\      \                                                              */
/*                    /'   (_/  _-~  | |__>--<__|      |                                                             */
/*                   |0  0 _/) )-~     | |__>--<__|     |                                                            */
/*                   / /~ ,_/       / /__>---<__/      |                                                             */
/*                  o o _//        /-~_>---<__-~      /                                                              */
/*                  (^(~          /~_>---<__-      _-~                                                               */
/*                 ,/|           /__>--<__/     _-~                                                                  */
/*              ,//('(          |__>--<__|     /                  .----_                                             */
/*             ( ( '))          |__>--<__|    |                 /' _---_~\                                           */
/*          `-)) )) (           |__>--<__|    |               /'  /     ~\`\                                         */
/*         ,/,'//( (             \__>--<__\    \            /'  //        ||                                         */
/*       ,( ( ((, ))              ~-__>--<_~-_  ~--____---~' _/'/        /'                                          */
/*     `~/  )` ) ,/|                 ~-_~>--<_/-__       __-~ _/                                                     */
/*   ._-~//( )/ )) `                    ~~-'_/_/ /~~~~~~~__--~                                                       */
/*    ;'( ')/ ,)(                              ~~~~~~~~~~                                                            */
/*   ' ') '( (/                                                                                                      */
/*     '   '  `                                                                                                      */
/*********************************************************************************************************************/
#ifndef _WINDOW_H_
#define _WINDOW_H_

#include <windows.h>
#include "input.h"

/* Window internal data */
struct wnd_internal
{
    HWND hwnd;       /* The window handle */
    HGLRC context;   /* The opengl context handle */
    HDC hdc;         /* The window device context */
    int keymap[512]; /* The keycode mappings */
};

/* Per window structure */
struct window
{
    /* The internal window data */
    struct wnd_internal internal;
    /* Holds the pressed state of the keyboard keys */
    char keys[KEY_LAST + 1];
    /* Holds the pressed state of the mouse keys */
    char mouse_keys[MOUSE_BTN_LAST + 1];
    /* Holds the pressed state of the joystick keys */
    char joystick_keys[JOYSTICK_BUTTON_LAST + 1];
    /* Non zero when window marked for closing */
    int should_close;
};

/* Opens a window */
void open_window(struct window* w);

/* Polls window for system events */
void poll_window_events(struct window* w);

/* Closes window and releases its resources */
void close_window(struct window* w);

/* Returns if window is marked for closing */
int window_should_close(struct window* wnd);

/* Swaps front and back buffer */
void swap_buffers(struct window* wnd);

/* Checks the pressed state of the given key */
int is_key_pressed(struct window* wnd, enum key k);

#endif // ! _WINDOW_H_
