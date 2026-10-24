#ifndef CUBEVERTVESH_H
#define CUBEVERTVESH_H
#include "cube.h"
#include "vert_animation.h"
#include "modelcache.h"
#include "tristrip.h"

#include "maths/aabb.h"
#include "maths/vector_maths.h"
#include "maths/matrix_maths.h"
#include "texture.h"

struct tcvert { 
    float u, v; 
};

struct lightvert { 
    uchar r, g, b, a; 
};

struct tri { 
    ushort vert[3]; 
    ushort neighbor[3]; 
};

struct part;

struct dyncacheentry : modelcacheentry<dyncacheentry> {
    anpos cur, prev;
    float t;

    vec3_t *verts() { return (vec3_t *)getdata(); }
    int numverts() { return int((size - sizeof(dyncacheentry)) / sizeof(vec3_t)); }
};

struct shadowcacheentry : modelcacheentry<shadowcacheentry> {
    anpos cur, prev;
    float t;
    vec3_t dir;

    ushort *idxs() { return (ushort *)getdata(); }
    int numidxs() { return int((size - sizeof(shadowcacheentry)) / sizeof(ushort)); }
};

struct lightcacheentry : modelcacheentry<lightcacheentry> {
    anpos cur, prev;
    float t;
    int lastcalclight;
    vec3_t pos;
    float yaw, pitch;

    lightvert *verts() { return (lightvert *)getdata(); }
    int numverts() { return int((size - sizeof(lightcacheentry)) / sizeof(lightvert)); }
};

struct mesh {
    char* name;
    part* owner;
    vec3_t* verts;
    tcvert* tcverts;
    aabb_s* bbs;
    ushort* shareverts;
    tri* tris;
    int numverts, numtris;

    Texture* skin;
    int tex;

    modelcachelist<dyncacheentry> dyncache;
    modelcachelist<shadowcacheentry> shadowcache;
    modelcachelist<lightcacheentry> lightcache;

    ushort *dynidx;
    int dynlen;
    drawcall *dyndraws;
    int numdyndraws;
    GLuint statlist;
    int statlen;

    mesh();
    ~mesh();

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