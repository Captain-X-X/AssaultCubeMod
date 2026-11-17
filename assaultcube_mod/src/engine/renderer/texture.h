#ifndef TEXTURE_H
#define TEXTURE_H
#pragma once
#include "pch.h"
#include "tools/cube_hashtable.h"
#include "tools/tools.h"

class Texture
{
public:
    char *name;
    int xs, ys, bpp, clamp;
    float scale;
    bool mipmap, canreduce;
    unsigned int id;
    bool loaded;

public:
    Texture();
    ~Texture();

    bool load(const char *name, int clamp = 0, bool mipmap = true, bool canreduce = false, float scale = 1.0f, bool trydl = false);
    bool load();
    void destroy();
};

struct textureSlot_t {
    string name;
    float scale;
    Texture* tex;
    bool loaded;
};

extern Texture *notexture, *noworldtexture;
extern bool silent_texture_load;
extern bool uniformtexres;
class TextureManager
{
public:
    hashtable<char *, Texture> textures;
    vector<textureSlot_t> slots;
    
public:
    TextureManager();
    ~TextureManager();
    void Destroy();
    
    Texture* load(const char *name, int clamp = 0, bool mipmap = true, bool canreduce = false, float scale = 1.0f, bool trydl = false);
    Texture* get(int tex, Texture* failtex = notexture, bool trydl = false);
    bool reload(Texture &t);
    bool reload(const char *name);
    void reloadAll();
};
extern TextureManager g_TextureMgr;

void scaletexture(uchar *src, uint sw, uint sh, uint bpp, uchar *dst, uint dw, uint dh);
void createtexture(int tnum, int w, int h, void *pixels, int clamp, bool mipmap, bool canreduce, GLenum format);
Texture *textureload(const char *name, int clamp = 0, bool mipmap = true, bool canreduce = false, float scale = 1.0f, bool trydl = false);
Texture *lookuptexture(int tex, Texture *failtex = notexture, bool trydl = false);
bool reloadtexture(Texture &t);
bool reloadtexture(const char *name);
void reloadtextures();
static inline Texture *lookupworldtexture(int tex, bool trydl = true)
{ return lookuptexture(tex, noworldtexture, trydl); }

/*
* ==========
* 
* ==========
*/
struct zone 
{ 
    int x1, x2, y1, y2, color; 
}; // zones (drawn on the minimap)

extern int maxtmus;
extern void inittmus();
extern void resettmu(int n);
extern void scaletmu(int n, int rgbscale, int alphascale = 0);
extern void colortmu(int n, float r = 0, float g = 0, float b = 0, float a = 0);
extern void setuptmu(int n, const char *rgbfunc = NULL, const char *alphafunc = NULL);

/*
* ==========
* 
* ==========
*/
void loadsky(char *basename, bool reload);
extern float skyfloor;
extern void draw_envbox(int fogdist);

#endif