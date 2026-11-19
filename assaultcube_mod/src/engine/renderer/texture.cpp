#include "pch.h"
#include "cube.h"
#include "texture.h"
#include "api/gl/glutil.h"

#include "client/client.h"
#include "lib/stb_image.h"

TextureManager g_TextureMgr;

#define FUNCNAME(name) name##1
#define DEFPIXEL uint OP(r, 0);
#define PIXELOP OP(r, 0);
#define BPP 1
#include "scale.h"

#define FUNCNAME(name) name##2
#define DEFPIXEL uint OP(r, 0), OP(g, 1);
#define PIXELOP OP(r, 0); OP(g, 1);
#define BPP 2
#include "scale.h"

#define FUNCNAME(name) name##3
#define DEFPIXEL uint OP(r, 0), OP(g, 1), OP(b, 2);
#define PIXELOP OP(r, 0); OP(g, 1); OP(b, 2);
#define BPP 3
#include "scale.h"

#define FUNCNAME(name) name##4
#define DEFPIXEL uint OP(r, 0), OP(g, 1), OP(b, 2), OP(a, 3);
#define PIXELOP OP(r, 0); OP(g, 1); OP(b, 2); OP(a, 3);
#define BPP 4
#include "scale.h"

void scaletexture(uchar *src, uint sw, uint sh, uint bpp, uchar *dst, uint dw, uint dh)
{
    if(sw == dw*2 && sh == dh*2) {
        switch(bpp) {
        case 1: return halvetexture1(src, sw, sh, dst);
        case 2: return halvetexture2(src, sw, sh, dst);
        case 3: return halvetexture3(src, sw, sh, dst);
        case 4: return halvetexture4(src, sw, sh, dst);
        }
    }
    else if(sw < dw || sh < dh || sw&(sw-1) || sh&(sh-1)) {
        switch(bpp) {
        case 1: return scaletexture1(src, sw, sh, dst, dw, dh);
        case 2: return scaletexture2(src, sw, sh, dst, dw, dh);
        case 3: return scaletexture3(src, sw, sh, dst, dw, dh);
        case 4: return scaletexture4(src, sw, sh, dst, dw, dh);
        }
    }
    else {
        switch(bpp) {
        case 1: return shifttexture1(src, sw, sh, dst, dw, dh);
        case 2: return shifttexture2(src, sw, sh, dst, dw, dh);
        case 3: return shifttexture3(src, sw, sh, dst, dw, dh);
        case 4: return shifttexture4(src, sw, sh, dst, dw, dh);
        }
    }
}

Texture *notexture = NULL, *noworldtexture = NULL;
Texture notexture_noptr, noworldtexture_noptr;

VAR(hwtexsize, 1, 0, 0);
VAR(hwmaxaniso, 1, 0, 0);
VARFP(maxtexsize, 0, 0, 1<<12, initwarning("texture quality", INIT_LOAD));
VARFP(texreduce, -1, 0, 3, initwarning("texture quality", INIT_LOAD));
VARFP(trilinear, 0, 1, 1, initwarning("texture filtering", INIT_LOAD));
VARFP(bilinear, 0, 1, 1, initwarning("texture filtering", INIT_LOAD));
VARFP(aniso, 0, 0, 16, initwarning("texture filtering", INIT_LOAD));

void resizetexture(int w, int h, bool mipmap, bool canreduce, GLenum target, int &tw, int &th)
{
    int hwlimit = hwtexsize,
        sizelimit = mipmap && maxtexsize ? min(maxtexsize, hwlimit) : hwlimit;
    if(canreduce && texreduce) {
        if(texreduce==-1) {
            w = 2;
            h = 2;
        }
        else {
            w = max(w>>texreduce, 2); // 1);
            h = max(h>>texreduce, 2); // 1);
        }
    }
    w = min(w, sizelimit);
    h = min(h, sizelimit);
    if(mipmap || w&(w-1) || h&(h-1)) {
        tw = th = 1;
        while(tw < w) tw *= 2;
        while(th < h) th *= 2;
        if(w < tw - tw/4) tw /= 2;
        if(h < th - th/4) th /= 2;
    }
    else {
        tw = w;
        th = h;
    }
}

void createtexture(int tnum, int w, int h, void *pixels, int clamp, bool mipmap, bool canreduce, GLenum format)
{
    glBindTexture(GL_TEXTURE_2D, tnum);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, clamp&1 ? GL_CLAMP_TO_EDGE : GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, clamp&2 ? GL_CLAMP_TO_EDGE : GL_REPEAT);
    if(hasAF && min(aniso, hwmaxaniso) > 0 && mipmap) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, min(aniso, hwmaxaniso));
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, bilinear ? GL_LINEAR : GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
        mipmap ?
            (trilinear ?
                (bilinear ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST_MIPMAP_LINEAR) :
                (bilinear ? GL_LINEAR_MIPMAP_NEAREST : GL_NEAREST_MIPMAP_NEAREST)) :
            (bilinear ? GL_LINEAR : GL_NEAREST));

    int tw = w, th = h;
    if(pixels) {
        resizetexture(w, h, mipmap, canreduce, GL_TEXTURE_2D, tw, th);
    }
    ///////////////////
    unsigned int bpp = GL_GetTextureFormatSize(format);
    uchar *buf = NULL;
    if(w!=tw || h!=th) {
        buf = new uchar[tw*th*bpp];
        scaletexture((uchar *)pixels, w, h, bpp, buf, tw, th);
    }
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    for(int level = 0;; level++) {
        uchar *src = buf ? buf : (uchar *)pixels;
        glTexImage2D(GL_TEXTURE_2D, level, format, tw, th, 0, format, GL_UNSIGNED_BYTE, src);
        if(!mipmap || max(tw, th) <= 1) break;
        int srcw = tw, srch = th;
        if(tw > 1) tw /= 2;
        if(th > 1) th /= 2;
        if(!buf) buf = new uchar[tw*th*bpp];
        scaletexture(src, srcw, srch, bpp, buf, tw, th);
    }
    if(buf) {
        delete[] buf;
    }
}

bool silent_texture_load = false;
VARFP(hirestextures, 0, 1, 1, initwarning("texture resolution", INIT_LOAD));
bool uniformtexres = !hirestextures;

GLenum texformat(int bpp)
{
    switch(bpp) {
    case 8: return GL_LUMINANCE;
    case 16: return GL_LUMINANCE_ALPHA;
    case 24: return GL_RGB;
    case 32: return GL_RGBA;
    default: return 0;
    }
}

GLuint loadsurface(const char *texname, int &xs, int &ys, int &bpp, int clamp = 0, bool mipmap = true, bool canreduce = false, float scale = 1.0f, bool trydl = false)
{
    unsigned char* data = stbi_load(texname, &xs, &ys, &bpp, 0);
    if (!data) {
        // printf("Failed to load texture: %s\n", texname);
        return 0;
    }
    bpp = bpp*8;
    GLenum format = GL_RGBA;
    format = texformat(bpp);
    GLuint id;
    glGenTextures(1, &id);
    createtexture(id, xs, ys, (void*)data, clamp, mipmap, canreduce, format);
    stbi_image_free(data);
    return id;
}

void texturereset() 
{ 
    if(execcontext==IEXC_MAPCFG) {
        g_TextureMgr.reset(true); 
    }
}
COMMAND(texturereset, "");

void texture(float *scale, char *name)
{
    std::string textureFilePath = "packages/textures/" + std::string(name);
    Texture tex0;
    tex0.preset(textureFilePath, 0, true, false, (*scale > 0 && *scale <= 2.0f) ? *scale : 1.0f);
    g_TextureMgr.add(tex0, true);
}
COMMAND(texture, "fs");

/*
* ==========
* 
* ==========
*/
void Texture::scaleTexture()
{
    uint dw = m_width * m_scale, dh = m_height * m_scale;
    uchar *buf = new uchar[dw*dh*m_bpp];
    scaletexture(m_pixels, m_width, m_height, m_bpp, buf, dw, dh);
    // delete[] (uchar *)s->pixels;
    m_width = dw;
    m_height = dh;
    m_pixels = buf;
}

Texture::Texture()
    : m_id(0), m_preset(false)
{

}

Texture::~Texture()
{
    // destroy();
}

void Texture::preset(const std::string& file, int clamp, bool mipmap, bool canreduce, float scale)
{
    m_file = file;
    m_clamp = clamp;
    m_mipmap = mipmap;
    m_canreduce = canreduce;
    m_id = 0;
    m_scale = scale;
    m_preset = true;
}

bool Texture::load(const std::string& file, int clamp, bool mipmap, bool canreduce, float scale)
{
    m_file = file;
    m_clamp = clamp;
    m_mipmap = mipmap;
    m_canreduce = canreduce;
    
    unsigned char* data = stbi_load(file.c_str(), &m_width, &m_height, &m_bpp, 0);
    if (!data) {
        printf("fail to load texture: %s\n", file.c_str());
        return false;
    }
    
    m_bpp = m_bpp*8;
    GLenum format = GL_RGBA;
    format = texformat(m_bpp);
    glGenTextures(1, &m_id);
    createtexture(m_id, m_width, m_height, (void*)data, clamp, mipmap, canreduce, format);

    // m_scale = scale / 1.0f;
    // if(uniformtexres && scale > 1.0f) scaleTexture();
    m_scale = scale;

    stbi_image_free(data);

    if(m_id == NULL) {
        // destroy();
        return false;
    }
    return true;
}

bool Texture::load()
{
    if (m_preset) {
        m_preset = false;
        return load(m_file, m_clamp, m_mipmap, m_canreduce, m_scale);
    }
    return false;
}

bool Texture::reload()
{
    if (m_file.empty()) {
        return false;
    }

    if (isValid()) {
        destroy();
    }
    return load(m_file.c_str(), m_clamp, m_mipmap, m_canreduce);
}

void Texture::destroy()
{
    if (m_id != 0) {
        // printf("texture: %d destroyed!\n", m_id);
        glDeleteTextures(1, &m_id);
        m_id = 0;
    }
}

bool Texture::isValid() const
{
    if (m_id == NULL) {
        return false;
    }
    return true;
}

bool Texture::isPreset() const
{
    return m_preset;
}

unsigned int Texture::getId() const
{
    return m_id;
}

const std::string& Texture::getFileName() const
{
    return m_file;
}

/*
* ==========
* 
* ==========
*/
TextureManager::TextureManager()
    : m_mloutput(false)
{

}

TextureManager::~TextureManager()
{
    shutdown();
}

void TextureManager::shutdown()
{
    for (size_t i = 0; i < m_textures.size(); i++) {
        if (m_textures[i]) {
            m_textures[i]->destroy();
        }
    }
}

void TextureManager::reset(bool delayedLoadTexture, bool deleteTextures)
{
    if (delayedLoadTexture) {
        m_texturesDelayedLoad.clear();
    }
    else {
        m_textures.clear();
    }
    printf("================texture manager has reset!\n");
}

void TextureManager::add(Texture& tex, bool isDelayedLoadTexture)
{
    if (isDelayedLoadTexture) {
        m_texturesDelayedLoad.push_back(std::make_shared<Texture>(tex));
        return;
    }
    m_textures.push_back(std::make_shared<Texture>(tex));
}

Texture* TextureManager::load(const std::string& file, int clamp, bool mipmap, bool canreduce, float scale)
{
    Texture* pTextureFind = getByFileName(file);
    if (pTextureFind != nullptr){
        return pTextureFind;
    }

    Texture tex0;
    if (!tex0.load(file, clamp, mipmap, canreduce, scale)) {
        return notexture;
    }
    m_textures.push_back(std::make_shared<Texture>(tex0));
    printf("[%ld] added texture: %s\n", m_textures.size(), file.c_str());
    return m_textures.back().get();
}

Texture* TextureManager::get(unsigned int textureIndex)
{
    if (m_textures.size() > textureIndex) {
        if (m_textures[textureIndex]->isPreset()) {
            m_textures[textureIndex]->load();
        }
        return m_textures[textureIndex].get();
    }

    if (m_mloutput) {
        printf("check: failed to get texture: %d ... %d\n", textureIndex, m_textures.size());
    }
    return nullptr;
}

Texture* TextureManager::getByFileName(const std::string& fileName)
{
    for (size_t i = 0; i < m_textures.size(); i++) {
        if (m_textures[i]) {
            // printf("texture check = %s != %s\n", fileName.c_str(), m_textures[i]->getFileName().c_str());
            if (m_textures[i]->getFileName() == fileName) {
                // return m_textures[i].get();
            }
        }
    }
    if (m_mloutput) {
        printf("check: failed to get texture: %s\n", fileName.c_str());
    }
    return nullptr;
}

Texture* TextureManager::getDelayedLoadTexture(unsigned int textureIndex)
{
    if (m_texturesDelayedLoad.size() > textureIndex) {
        if (m_texturesDelayedLoad[textureIndex]->isPreset()) {
            m_texturesDelayedLoad[textureIndex]->load();
        }
        return m_texturesDelayedLoad[textureIndex].get();
    }

    if (m_mloutput) {
        printf("check dlt: failed to get texture: %d ... %d\n", textureIndex, m_texturesDelayedLoad.size());
    }
    return nullptr;
}

bool TextureManager::reload(Texture &t)
{
    if(t.isValid()) {
        return true;
    }
    return t.reload();
}

bool TextureManager::reload(const char *name)
{
    for (size_t i = 0; i < m_textures.size(); i++) {
        if (m_textures[i]) {
            if (m_textures[i]->getFileName() == name) {
                return m_textures[i]->reload();
            }
        }
    }
    return false;
}

void TextureManager::reloadAll()
{
    for (size_t i = 0; i < m_textures.size(); i++) {
        if (m_textures[i]) {
            m_textures[i]->reload();
        }
    }
}

Texture* lookupworldtexture(int tex, bool trydl)
{
    Texture* pReturnTexture = g_TextureMgr.getDelayedLoadTexture(tex);
    if (pReturnTexture == nullptr) {
        return noworldtexture;
    }
    return pReturnTexture;
}

Texture sky[6];
static string skybox;

void loadsky(char *basename, bool reload)
{
    const char *side[] = { "lf", "rt", "ft", "bk", "dn", "up" };
    if(reload) basename = skybox;
    else copystring(skybox, basename);
    loopi(6)
    {
        defformatstring(name)("packages/%s_%s.jpg", basename, side[i]);
        sky[i].load(name, 3);
        // if(sky[i] == notexture && !reload && autodownload)
        // {
        //     // defformatstring(dl)("packages/%s", basename);
        //     // requirepackage(PCK_SKYBOX, dl);
        //     break;
        // }
    }
}
COMMANDF(loadsky, "s", (char *name) { loadsky(name, false); intret(0); });

void loadnotexture(char *c)
{
    noworldtexture = notexture; // reset to default
    if(c[0]) {
        defformatstring(p)("packages/textures/%s", c);
        noworldtexture_noptr.load(p);
        noworldtexture = &noworldtexture_noptr;
        if(noworldtexture==notexture) {
            conoutf("could not load alternative texture '%s'.", p);
        }
    }
}
COMMAND(loadnotexture, "s");

void draw_envbox_face(float s0, float t0, float x0, float y0, float z0,
                      float s1, float t1, float x1, float y1, float z1,
                      float s2, float t2, float x2, float y2, float z2,
                      float s3, float t3, float x3, float y3, float z3,
                      Texture *tex)
{
    glBindTexture(GL_TEXTURE_2D, tex->getId());
    glBegin(GL_TRIANGLE_STRIP);
    glTexCoord2f(s3, t3); glVertex3f(x3, y3, z3);
    glTexCoord2f(s2, t2); glVertex3f(x2, y2, z2);
    glTexCoord2f(s0, t0); glVertex3f(x0, y0, z0);
    glTexCoord2f(s1, t1); glVertex3f(x1, y1, z1);
    glEnd();
    xtraverts += 4;
}

VAR(skyclip, 0, 1, 1);

float skyfloor = 1e16f;

void draw_envbox(int w)
{
    extern float skyfloor;

    float zclip = skyclip && skyfloor >= camera1->o.z ? 0.5f + float(skyfloor-camera1->o.z)/w : 0.0f,
          vclip = 1-zclip,
          z = 2*w*(vclip-0.5f);

    if(vclip < 0) return;

    glDepthMask(GL_FALSE);

    draw_envbox_face(0.0f, 0.0f, -w, -w, -w,
                     1.0f, 0.0f, -w,  w, -w,
                     1.0f, vclip, -w,  w,  z,
                     0.0f, vclip, -w, -w,  z, sky[0].isValid() ? &sky[0] : notexture);

    draw_envbox_face(1.0f, vclip, +w, -w,  z,
                     0.0f, vclip, +w,  w,  z,
                     0.0f, 0.0f, +w,  w, -w,
                     1.0f, 0.0f, +w, -w, -w, sky[1].isValid() ? &sky[1] : notexture);

    draw_envbox_face(1.0f, vclip, -w, -w,  z,
                     0.0f, vclip,  w, -w,  z,
                     0.0f, 0.0f,  w, -w, -w,
                     1.0f, 0.0f, -w, -w, -w, sky[2].isValid() ? &sky[2] : notexture);

    draw_envbox_face(1.0f, vclip, +w,  w,  z,
                     0.0f, vclip, -w,  w,  z,
                     0.0f, 0.0f, -w,  w, -w,
                     1.0f, 0.0f, +w,  w, -w, sky[3].isValid() ? &sky[3] : notexture);

    if(!zclip)
        draw_envbox_face(0.0f, 1.0f, -w,  w,  w,
                         0.0f, 0.0f, +w,  w,  w,
                         1.0f, 0.0f, +w, -w,  w,
                         1.0f, 1.0f, -w, -w,  w, sky[4].isValid() ? &sky[4] : notexture);

    draw_envbox_face(0.0f, 1.0f, +w,  w, -w,
                     0.0f, 0.0f, -w,  w, -w,
                     1.0f, 0.0f, -w, -w, -w,
                     1.0f, 1.0f, +w, -w, -w, sky[5].isValid() ? &sky[5] : notexture);

    glDepthMask(GL_TRUE);

    skyfloor = 1e16f;
}

struct tmufunc
{
    GLenum combine, sources[3], ops[3];
    int scale;
};

struct tmu
{
    GLenum mode;
    GLfloat color[4];
    tmufunc rgb, alpha;
};

#define INVALIDTMU \
{ \
    0, \
    { -1, -1, -1, -1 }, \
    { 0, { 0, 0, 0 }, { 0, 0, 0 }, 0 }, \
    { 0, { 0, 0, 0 }, { 0, 0, 0 }, 0 } \
}

#define INITTMU \
{ \
    GL_MODULATE, \
    { 0, 0, 0, 0 }, \
    { GL_MODULATE, { GL_TEXTURE, GL_PREVIOUS_ARB, GL_CONSTANT_ARB }, { GL_SRC_COLOR, GL_SRC_COLOR, GL_SRC_ALPHA }, 1 }, \
    { GL_MODULATE, { GL_TEXTURE, GL_PREVIOUS_ARB, GL_CONSTANT_ARB }, { GL_SRC_ALPHA, GL_SRC_ALPHA, GL_SRC_ALPHA }, 1 } \
}

#define MAXTMUS 4

tmu tmus[MAXTMUS] =
{
    INVALIDTMU,
    INVALIDTMU,
    INVALIDTMU,
    INVALIDTMU
};

VAR(maxtmus, 1, 0, 0);

void parsetmufunc(tmufunc &f, const char *s)
{
    int arg = -1;
    while(*s) {
        switch(tolower(*s++)) {
        case 't': f.sources[++arg] = GL_TEXTURE; f.ops[arg] = GL_SRC_COLOR; break;
        case 'p': f.sources[++arg] = GL_PREVIOUS_ARB; f.ops[arg] = GL_SRC_COLOR; break;
        case 'k': f.sources[++arg] = GL_CONSTANT_ARB; f.ops[arg] = GL_SRC_COLOR; break;
        case 'c': f.sources[++arg] = GL_PRIMARY_COLOR_ARB; f.ops[arg] = GL_SRC_COLOR; break;
        case '~': f.ops[arg] = GL_ONE_MINUS_SRC_COLOR; break;
        case 'a': f.ops[arg] = f.ops[arg]==GL_ONE_MINUS_SRC_COLOR ? GL_ONE_MINUS_SRC_ALPHA : GL_SRC_ALPHA; break;
        case '=': f.combine = GL_REPLACE; break;
        case '*': f.combine = GL_MODULATE; break;
        case '+': f.combine = GL_ADD; break;
        case '-': f.combine = GL_SUBTRACT_ARB; break;
        case ',':
        case '@': f.combine = GL_INTERPOLATE_ARB; break;
        case '.': f.combine = GL_DOT3_RGB_ARB; break;
        case 'x': while(!isdigit(*s)) s++; f.scale = *s++-'0'; break;
        }
    }
}

void committmufunc(bool rgb, tmufunc &dst, tmufunc &src)
{
    if(dst.combine!=src.combine) glTexEnvi(GL_TEXTURE_ENV, rgb ? GL_COMBINE_RGB_ARB : GL_COMBINE_ALPHA_ARB, src.combine);
    for (int i = 0; i < 3; i++) {
        if(dst.sources[i]!=src.sources[i]) glTexEnvi(GL_TEXTURE_ENV, (rgb ? GL_SOURCE0_RGB_ARB : GL_SOURCE0_ALPHA_ARB)+i, src.sources[i]);
        if(dst.ops[i]!=src.ops[i]) glTexEnvi(GL_TEXTURE_ENV, (rgb ? GL_OPERAND0_RGB_ARB : GL_OPERAND0_ALPHA_ARB)+i, src.ops[i]);
    }
    if(dst.scale!=src.scale) glTexEnvi(GL_TEXTURE_ENV, rgb ? GL_RGB_SCALE_ARB : GL_ALPHA_SCALE, src.scale);
}

void committmu(int n, tmu &f)
{
    if(n>=maxtmus) return;
    if(tmus[n].mode!=f.mode) glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, f.mode);
    if(memcmp(tmus[n].color, f.color, sizeof(f.color))) glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, f.color);
    committmufunc(true, tmus[n].rgb, f.rgb);
    committmufunc(false, tmus[n].alpha, f.alpha);
    tmus[n] = f;
}

void resettmu(int n)
{
    tmu f = tmus[n];
    f.mode = GL_MODULATE;
    f.rgb.scale = 1;
    f.alpha.scale = 1;
    committmu(n, f);
}

void scaletmu(int n, int rgbscale, int alphascale)
{
    tmu f = tmus[n];
    if(rgbscale) f.rgb.scale = rgbscale;
    if(alphascale) f.alpha.scale = alphascale;
    committmu(n, f);
}

void colortmu(int n, float r, float g, float b, float a)
{
    tmu f = tmus[n];
    f.color[0] = r;
    f.color[1] = g;
    f.color[2] = b;
    f.color[3] = a;
    committmu(n, f);
}

void setuptmu(int n, const char *rgbfunc, const char *alphafunc)
{
    static tmu init = INITTMU;
    tmu f = tmus[n];

    f.mode = GL_COMBINE_ARB;
    if(rgbfunc) parsetmufunc(f.rgb, rgbfunc);
    else f.rgb = init.rgb;
    if(alphafunc) parsetmufunc(f.alpha, alphafunc);
    else f.alpha = init.alpha;

    committmu(n, f);
}

void inittmus()
{
    GLint val;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &val);
    hwtexsize = val;

    if(hasTE && !hasMT) {
        maxtmus = 1;
    }
    else if(hasTE && hasMT) {
        glGetIntegerv(GL_MAX_TEXTURE_UNITS_ARB, &val);
        maxtmus = max(1, min(MAXTMUS, int(val)));
        for (int i = 0; i < maxtmus; i++) {
            glActiveTexture(GL_TEXTURE0_ARB+i);
            resettmu(i);
        }
        glActiveTexture(GL_TEXTURE0_ARB);
    }
}

void cleanuptmus()
{
    tmu invalidtmu = INVALIDTMU;
    loopi(MAXTMUS) tmus[i] = invalidtmu;
}

Texture *e_wall = NULL, *e_floor = NULL, *e_ceil = NULL;

void guidetoggle()
{
    if(player1->state == CS_EDITING) {
        // Texture *sw = &g_TextureMgr.slots[DEFAULT_WALL];
        // Texture *sf = &g_TextureMgr.slots[DEFAULT_FLOOR];
        // Texture *sc = &g_TextureMgr.slots[DEFAULT_CEIL];

        //if textures match original texture
        if(e_wall == NULL || e_floor == NULL || e_ceil == NULL) {
            //replace defaults with grid texures
            e_wall = g_TextureMgr.load("packages/textures/map_editor/wall.png");
            e_floor = g_TextureMgr.load("packages/textures/map_editor/floor.png");
            e_ceil = g_TextureMgr.load("packages/textures/map_editor/ceil.png");
            conoutf("Guide: \f0on");
        }
        else {
            // // restore textures
            // if(e_wall) sw->tex = e_wall;
            // if(e_floor) sf->tex = e_floor;
            // if(e_ceil) sc->tex = e_ceil;
            e_wall = NULL;
            e_floor = NULL;
            e_ceil = NULL;
            conoutf("Guide: \fBoff");
        }
    }
    else {
        // conoutf("\fBGuide view is only avaiable when editing.");
    }
}

COMMAND(guidetoggle, "");