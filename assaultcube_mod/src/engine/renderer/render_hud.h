#ifndef RENDERHUD_H
#define RENDERHUD_H
#include "cube.h"

enum
{
    HUDMSG_INFO = 0,
    HUDMSG_TIMER,

    HUDMSG_TYPE = 0xFF,
    HUDMSG_OVERWRITE = 1<<8
};
void gl_drawhud(int w, int h, int curfps, int nquads, int curvert, bool underwater);
void loadingscreen(const char *fmt = NULL, ...);
void hudoutf(const char *s, ...);
void hudonlyf(const char *s, ...);
void hudeditf(int type, const char *s, ...);
void show_out_of_renderloop_progress(float bar1, const char *text1, float bar2 = 0, const char *text2 = NULL);
void updatedmgindicator(vec &attack);
vec getradarpos();

#endif