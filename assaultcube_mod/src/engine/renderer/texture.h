#ifndef TEXTURE_H
#define TEXTURE_H
#pragma once
#include "pch.h"
#include "tools/cube_hashtable.h"
#include "tools/tools.h"

class Texture
{
private:
    unsigned char* m_pixels; // deleted when the texture processing is done 
    unsigned int m_id;
    int m_bpp;
    int m_width;
    int m_height;
    int m_clamp;
    float m_scale;
    bool m_mipmap;
    bool m_canreduce;
    bool m_preset;
    std::string m_file;

    void scaleTexture();

public:
    Texture();
    ~Texture();

    // this is not to load NOW.. but delay until its needed
    void preset(const std::string& file, int clamp = 0, bool mipmap = true, bool canreduce = false, float scale = 1.0f);
    
    // load NOW...
    bool load(const std::string& file, int clamp = 0, bool mipmap = true, bool canreduce = false, float scale = 1.0f);
    bool load();
    bool reload();
    void destroy();

    // getters
    bool isValid() const;
    bool isPreset() const;
    unsigned int getId() const;
    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }
    float getScale() const { return m_scale; }
    int getBytesPerPixle() const { return m_bpp; }
    const std::string& getFileName() const;
};

extern Texture *notexture, *noworldtexture;
extern bool silent_texture_load;
extern bool uniformtexres;

// struct TextureDelayedLoadInfo_s {
//     int m_bpp;
//     int m_width;
//     int m_height;
//     int m_clamp;
//     float m_scale;
//     bool m_mipmap;
//     bool m_canreduce;
//     std::string m_file;
//     bool m_loaded;
// };
class TextureManager
{
public:
    std::vector<std::shared_ptr<Texture>> m_textures;
    std::vector<std::shared_ptr<Texture>> m_texturesDelayedLoad; // added and then actual API texture created later...
    bool m_mloutput;

public:
    TextureManager();
    ~TextureManager();
    
    void shutdown();
    void reset(bool delayedLoadTexture, bool deleteTextures = true);
    
    void add(Texture& tex, bool isDelayedLoadTexture);
    Texture* load(const std::string& file, int clamp = 0, bool mipmap = true, bool canreduce = false, float scale = 1.0f);
    Texture* get(unsigned int textureIndex);
    Texture* getByFileName(const std::string& fileName);
    Texture* getDelayedLoadTexture(unsigned int textureIndex);

    bool reload(Texture &t);
    bool reload(const char *name);
    void reloadAll();
};
extern TextureManager g_TextureMgr;

void scaletexture(uchar *src, uint sw, uint sh, uint bpp, uchar *dst, uint dw, uint dh);
void createtexture(int tnum, int w, int h, void *pixels, int clamp, bool mipmap, bool canreduce, GLenum format);
// Texture *textureload(const char *name, int clamp = 0, bool mipmap = true, bool canreduce = false, float scale = 1.0f, bool trydl = false);
// Texture *lookuptexture(int tex, Texture *failtex = notexture, bool trydl = false);
// bool reloadtexture(Texture &t);
// bool reloadtexture(const char *name);
// void reloadtextures();
Texture* lookupworldtexture(int tex, bool trydl = true);

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