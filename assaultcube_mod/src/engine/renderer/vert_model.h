#ifndef CUBEVERTMODEL_H
#define CUBEVERTMODEL_H
#include "cube.h"
#include "modelcache.h"
#include "vert_animation.h"
#include "vert_mesh.h"
#include "model.h"

struct vertmodel;
struct part
{
    char *filename;
    vertmodel *model;
    int index, numframes;
    vector<mesh *> meshes;
    vector<animinfo> *anims;
    linkedpart *links;
    tag *tags;
    int numtags;
    GLuint *shadows;
    float shadowrad;

    part();
    virtual ~part();
    void cleanup();
    bool link(part *link, const char *tag, vec3_t *pos = NULL);
    bool gentag(const char *name, int *verts, int numverts, mesh *m = NULL);
    bool addemitter(const char *tag, int type, int arg1 = 0, int arg2 = 0);
    void scaleverts(const float scale, const vec3_t &translate);
    void genstrips();
    virtual void getdefaultanim(animstate &as, int anim, int varseed);
    void gentagmatrix(anpos &cur, anpos *prev, float ai_t, int i, GLfloat *matrix);
    bool calcanimstate(int anim, int varseed, float speed, int basetime, dynent *d, animstate &as);
    void render(int anim, int varseed, float speed, int basetime, dynent *d);
    void setanim(int num, int frame, int range, float speed);
    virtual void begingenshadow();
    virtual void endgenshadow();
    void blurshadow(const uchar *in, uchar *out, uint size);
    void genshadow(int aasize, int frame, stream *f);

    struct shadowheader
    {
        ushort size, frames;
        float height, rad;
    };

    void genshadows(float height, float rad);
    bool loadshadows(const char *filename);
    void rendershadow(int anim, int varseed, float speed, int basetime, const vec3_t &o, float yaw);
    char *shadowfile();
    float calcradius();
    void calcneighbors();
    void calcbbs();
};

struct vertmodel : model
{

    bool loaded;
    char *loadname;
    vector<part*> parts;

    vertmodel(const char *name);
    ~vertmodel();

    char* name();
    void cleanup();
    bool link(part *link, const char *tag, vec3_t *pos = NULL);
    void setskin(int tex = 0);
    void genshadows(float height, float rad);
    bool hasshadows();
    float calcradius();
    void calcneighbors();
    void calcbbs();

    static bool enablealphablend, enablealphatest, enabledepthmask, enableoffset;
    static GLuint lasttex;
    static float lastalphatest;
    static void *lastvertexarray, *lasttexcoordarray, *lastcolorarray;
    static glmatrixf matrixstack[32];
    static int matrixpos;

    void startrender();
    void endrender();

    static modelcache dynalloc, statalloc;
};

#endif