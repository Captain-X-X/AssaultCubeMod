#ifndef CUBEVERTVESH_H
#define CUBEVERTVESH_H
#include "cube.h"
#include "types/VertexData.h"
#include "vert_animation.h"
#include "ModelCache.h"
#include "tristrip.h"

#include "maths/aabb.h"
#include "maths/vector_maths.h"
#include "maths/matrix_maths.h"
#include "texture.h"

/*
* ==========
* predef
* ==========
*/
class ModelPart;

/*
* ==========
* 
* ==========
*/
struct dyncacheentry : ModelCacheEntry<dyncacheentry> {
    anpos cur, prev;
    float t;

    Vector3* verts() { return (Vector3*)getdata(); }
    int numverts() { return int((size - sizeof(dyncacheentry)) / sizeof(Vector3)); }
};

struct shadowcacheentry : ModelCacheEntry<shadowcacheentry> {
    anpos cur, prev;
    float t;
    Vector3 dir;

    ushort *idxs() { return (ushort *)getdata(); }
    int numidxs() { return int((size - sizeof(shadowcacheentry)) / sizeof(ushort)); }
};

struct lightcacheentry : ModelCacheEntry<lightcacheentry> 
{
    anpos cur, prev;
    float t;
    int lastcalclight;
    Vector3 pos;
    float yaw, pitch;

    LightVertex* verts() { return (LightVertex*)getdata(); }
    int numverts() { return int((size - sizeof(lightcacheentry)) / sizeof(LightVertex)); }
};

/*
* ==========
* 
* ==========
*/
struct tri { 
    ushort vert[3]; 
    ushort neighbor[3]; 
};

class Mesh 
{
public:
    char* name;
    ModelPart* owner;
    std::vector<Vector3>    m_verts;
    std::vector<Vector2>    m_uvs;
    std::vector<aabb_s>     m_aabbs;
    std::vector<ushort>     m_shareverts;
    std::vector<tri>        m_tris;
    std::vector<ushort>     m_dynidx;
    std::vector<drawcall>   m_dyndraws;
    int numverts;
    int numtris;

    Texture* skin;
    int tex;

    ModelCacheList<dyncacheentry> dyncache;
    ModelCacheList<shadowcacheentry> shadowcache;
    ModelCacheList<lightcacheentry> lightcache;

    GLuint statlist;
    int statlen;

public:
    Mesh();
    ~Mesh();

    void genstrips();
    dyncacheentry *gendynverts(animstate &as, anpos &cur, anpos *prev, float ai_t);
    void weldverts();
    void findneighbors();
    void cleanup();
    shadowcacheentry *genshadowvolume(animstate &as, anpos &cur, anpos *prev, float ai_t, vec3_t *buf);
    lightcacheentry *lightvertexes(animstate &as, anpos &cur, anpos *prev, float ai_t, vec3_t *buf);
    void getcurbb(aabb_s& b, animstate &as, anpos &cur, anpos *prev, float ai_t);
    void render(animstate &as, anpos &cur, anpos *prev, float ai_t);
    int findvert(int axis, int dir);
    float calcradius();
    void calcneighbors();
    void calcbbs();
};

#endif