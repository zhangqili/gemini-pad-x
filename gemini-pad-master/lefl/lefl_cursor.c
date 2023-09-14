/*
 * lefl_cursor.c
 */
#include "lefl.h"

void fezui_cursor_move(fezui_cursor_t *c,fezui_cursor_t *tc)
{
    lefl_easing_pid(&(c->x),tc->x);
    lefl_easing_pid(&(c->y),tc->y);
    lefl_easing_pid(&(c->w),tc->w);
    lefl_easing_pid(&(c->h),tc->h);
}

inline void fezui_cursor_set(fezui_cursor_t *c,float x,float y,float w,float h)
{
    c->x=x;
    c->y=y;
    c->w=w;
    c->h=h;
}
