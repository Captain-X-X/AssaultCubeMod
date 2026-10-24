#ifndef RENDERTEXT_H
#define RENDERTEXT_H
#include "cube.h"
#include "texture.h"

struct font
{
    struct charinfo
    {
        short x, y, w, h;
    };

    struct utf8charinfo : charinfo
    {
        int code;
    };

    char *name;
    Texture *tex;
    vector<charinfo> chars;
    int defaultw, defaulth;
    int offsetx, offsety, offsetw, offseth;
    int skip;
};

#define VIRTH 1800
#define FONTH (curfont->defaulth)
#define PIXELTAB (10*curfont->defaultw)

extern int VIRTW; // virtual screen size for text & HUD
extern bool ignoreblinkingbit;
extern font *curfont;

extern void initfont();
extern bool setfont(const char *name);
extern font *getfont(const char *name);
extern void pushfont(const char *name);
extern void popfont();
extern void draw_text(const char *str, int left, int top, int r = 255, int g = 255, int b = 255, int a = 255, int cursor = -1, int maxwidth = -1);
extern void draw_textf(const char *fstr, int left, int top, ...);
extern int text_width(const char *str);
extern int text_visible(const char *str, int max);
extern void text_bounds(const char *str, int &width, int &height, int maxwidth = -1);
extern int text_visible(const char *str, int hitx, int hity, int maxwidth);
extern void text_pos(const char *str, int cursor, int &cx, int &cy, int maxwidth);
extern void text_startcolumns();
extern void text_endcolumns();

#endif