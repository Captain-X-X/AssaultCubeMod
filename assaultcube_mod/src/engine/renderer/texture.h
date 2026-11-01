#ifndef TEXTURE_H
#define TEXTURE_H
#include "pch.h"

struct Texture
{
    char *name;
    int xs, ys, bpp, clamp;
    float scale;
    bool mipmap, canreduce;
    GLuint id;
};
extern Texture *notexture, *noworldtexture;
extern bool silent_texture_load;
extern bool uniformtexres;

extern void scaletexture(uchar *src, uint sw, uint sh, uint bpp, uchar *dst, uint dw, uint dh);
extern void createtexture(int tnum, int w, int h, void *pixels, int clamp, bool mipmap, bool canreduce, GLenum format);
extern SDL_Surface *wrapsurface(void *data, int width, int height, int bpp);
extern SDL_Surface *creatergbsurface(int width, int height);
extern SDL_Surface *creatergbasurface(int width, int height);
extern SDL_Surface *forcergbsurface(SDL_Surface *os);
extern SDL_Surface *forcergbasurface(SDL_Surface *os);
extern Texture *textureload(const char *name, int clamp = 0, bool mipmap = true, bool canreduce = false, float scale = 1.0f, bool trydl = false);
extern Texture *lookuptexture(int tex, Texture *failtex = notexture, bool trydl = false);
extern bool reloadtexture(Texture &t);
extern bool reloadtexture(const char *name);
extern void reloadtextures();
Texture *createtexturefromsurface(const char *name, SDL_Surface *s);
extern void blitsurface(SDL_Surface *dst, SDL_Surface *src, int x, int y);
void loadsky(char *basename, bool reload);

static inline Texture *lookupworldtexture(int tex, bool trydl = true)
{ return lookuptexture(tex, noworldtexture, trydl); }

extern float skyfloor;
extern void draw_envbox(int fogdist);

extern int autodownload;
extern void setupcurl();
extern bool requirepackage(int type, const char *path);
extern int downloadpackages();
extern void sortpckservers();
extern void writepcksourcecfg();

extern int maxtmus;
extern void inittmus();
extern void resettmu(int n);
extern void scaletmu(int n, int rgbscale, int alphascale = 0);
extern void colortmu(int n, float r = 0, float g = 0, float b = 0, float a = 0);
extern void setuptmu(int n, const char *rgbfunc = NULL, const char *alphafunc = NULL);

struct zone { int x1, x2, y1, y2, color; }; // zones (drawn on the minimap)

#endif