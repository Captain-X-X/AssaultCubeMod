#ifndef MODEL_H
#define MODEL_H
#include "cube.h"
#include "modelcache.h"
#include "vert_animation.h"
#include "vert_mesh.h"

/*
* ==========
* pre def
* ==========
*/
struct model;
struct modelattach;
class dynent;

/*
* ==========
* base model part
* ==========
*/
struct part
{
    char *filename;
    model *model;
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

/*
* ==========
* base model
* ==========
*/
struct model
{
    bool cullface, vertexlight, alphablend;  //ALX Alpha channel models
    float alphatest, translucency, scale, radius, shadowdist;
    vec3_t translate;
    int cachelimit, batch;
    bool loaded;
    char *loadname;
    vector<part*> parts;
    
    static bool enablealphablend, enablealphatest, enabledepthmask, enableoffset;
    static GLuint lasttex;
    static float lastalphatest;
    static void *lastvertexarray, *lasttexcoordarray, *lastcolorarray;
    static glmatrixf matrixstack[32];
    static int matrixpos;
    static modelcache dynalloc, statalloc;

    //model() : cullface(true), vertexlight(false), alphatest(0.9f), translucency(0.25f), scale(1), radius(0), shadowdist(0), translate(0, 0, 0), cachelimit(8), batch(-1) {}
    // model();
    model(const char *name);
    virtual ~model();
    
    virtual bool load() = 0;
    virtual char *name();
    virtual int type() = 0;
    
    virtual void cleanup();
    
    virtual void render(int anim, int varseed, float speed, int basetime, const vec3_t &o, float yaw, float pitch, dynent *d, modelattach *a = NULL, float scale = 1.0f) = 0;
    virtual void setskin(int tex = 0);
    
    virtual void genshadows(float height, float rad);
    virtual void rendershadow(int anim, int varseed, float speed, int basetime, const vec3_t &o, float yaw, modelattach *a = NULL) {}
    virtual bool hasshadows();
    
    virtual void startrender();
    virtual void endrender();
    
    virtual bool link(part *link, const char *tag, vec3_t *pos = NULL);
    virtual float calcradius();
    virtual void calcneighbors();
    virtual void calcbbs();
};

struct mapmodelinfo { 
    int rad, h, zoff; 
    string name; 
    model *m; 
};

class ModelManager 
{
private:

public:
    ModelManager();
    ~ModelManager();

    void initilize();
    void shutdown();

    void addModel(const model& mdl);
};

#endif