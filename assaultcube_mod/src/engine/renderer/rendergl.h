#ifndef RENDERGL_H
#define RENDERGL_H
#include "cube.h"

struct color
{
    float r, g, b, alpha;
    color(){}
    color(float r, float g, float b) : r(r), g(g), b(b), alpha(1.0f) {}
    color(float r, float g, float b, float a) : r(r), g(g), b(b), alpha(a) {}
};

extern bool hasTE, hasMT, hasMDA, hasDRE, hasstencil, hasST2, hasSTW, hasSTS, hasAF;

extern glmatrixf mvmatrix, projmatrix, mvpmatrix, invmvmatrix, invmvpmatrix;
extern glmatrixf clipmatrix;
extern vec worldpos, camdir, camup, camright;

void resetcamera();

void gl_checkextensions();
void gl_init(int w, int h, int bpp, int depth, int fsaa);
void cleangl();
void enablepolygonoffset(GLenum type);
void disablepolygonoffset(GLenum type, bool restore = true);
void line(int x1, int y1, float z1, int x2, int y2, float z2);
void line(int x1, int y1, int x2, int y2, color *c = NULL);
void box(block &b, float z1, float z2, float z3, float z4);
void dot(int x, int y, float z);
void linestyle(float width, int r, int g, int b);
void blendbox(int x1, int y1, int x2, int y2, bool border, int tex = -1, color *c = NULL);
void quad(GLuint tex, float x, float y, float s, float tx, float ty, float tsx, float tsy = 0);
void quad(GLuint tex, vec &c1, vec &c2, float tx, float ty, float tsx, float tsy);
void circle(GLuint tex, float x, float y, float r, float tx, float ty, float tr, int subdiv = 32);
void setperspective(float fovy, float aspect, float nearplane, float farplane);
void sethudgunperspective(bool on);
void gl_drawframe(int w, int h, float changelod, float curfps);
void clearminimap();
void resetzones();
void rendercursor(int x, int y, int w);
void renderaboveheadicon(playerent *p);
void drawscope(bool preload = false);
float dynfov();
void damageblend(int n);

enum
{
    CROSSHAIR_DEFAULT = 0,
    CROSSHAIR_TEAMMATE,
    CROSSHAIR_SCOPE,
    CROSSHAIR_KNIFE,
    CROSSHAIR_PISTOL,
    CROSSHAIR_CARBINE,
    CROSSHAIR_SHOTGUN,
    CROSSHAIR_SMG,
    CROSSHAIR_SNIPER,
    CROSSHAIR_AR,
    CROSSHAIR_CPISTOL,
    CROSSHAIR_GRENADES,
    CROSSHAIR_AKIMBO,
    CROSSHAIR_NUM,
};

void drawcrosshair(playerent *p, int n, color *c = NULL, float size = -1.0f);


#endif