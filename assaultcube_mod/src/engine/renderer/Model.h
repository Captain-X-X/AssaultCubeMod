#ifndef MODEL_H
#define MODEL_H
#include "cube.h"
#include "Mesh3D.h"
#include "ModelCache.h"
#include "vert_animation.h"

/*
* ==========
* pre def
* ==========
*/
class Model;
struct modelattach;
class dynent;

/*
* ==========
* base model part
* ==========
*/
class ModelPart
{
public:
    char* filename;
    Model* model;
    int index, numframes;
    std::vector<Mesh*> m_meshes;
    std::vector<std::vector<animinfo>> m_anims;
    std::vector<linkedpart> m_links;
    std::vector<tag> m_tags;
    int numtags;
    std::vector<GLuint> m_shadows;
    float shadowrad;

public:
    ModelPart();
    virtual ~ModelPart();
    void cleanup();

    bool link(ModelPart* pLink, const char* tag, vec3_t* pos = NULL);
    bool gentag(const char *name, int *verts, int numverts, Mesh* m = NULL);
    bool addemitter(const char *tag, int type, int arg1 = 0, int arg2 = 0);
    void scaleverts(const float scale, const vec3_t& translate);
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

    struct ShadowHeader {
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
enum class ModelTypes_t : unsigned char {
    NONE,
    MDL_MD2,
    MDL_MD3,
};
class Model
{
private:
    ModelTypes_t m_type;

public:
    bool cullface, vertexlight, alphablend;  //ALX Alpha channel models
    float alphatest, translucency, scale, radius, shadowdist;
    vec3_t translate;
    int cachelimit, batch;
    bool loaded;
    char *loadname;
    vector<ModelPart*> parts;
    
    static bool enablealphablend, enablealphatest, enabledepthmask, enableoffset;
    static GLuint lasttex;
    static float lastalphatest;
    static void *lastvertexarray, *lasttexcoordarray, *lastcolorarray;
    static Matrix4 matrixstack[32];
    static int matrixpos;
    static ModelCache dynalloc, statalloc;

public:
    Model();
    Model(const char *name, ModelTypes_t mtype);
    virtual ~Model();
    
    virtual bool load() { return false; };
    virtual char *name();
    
    virtual void cleanup();
    
    virtual void render(int anim, int varseed, float speed, int basetime, const vec3_t &o, float yaw, float pitch, dynent *d, modelattach *a = NULL, float scale = 1.0f);
    virtual void setskin(int tex = 0);
    
    virtual void genshadows(float height, float rad);
    virtual void rendershadow(int anim, int varseed, float speed, int basetime, const vec3_t &o, float yaw, modelattach *a = NULL) {}
    virtual bool hasshadows();
    
    virtual void startrender();
    virtual void endrender();
    
    virtual bool link(ModelPart* pLink, const char *tag, vec3_t *pos = NULL);
    virtual float calcradius();
    virtual void calcneighbors();
    virtual void calcbbs();

    // getters
    ModelTypes_t getType() const { return m_type; }
};

struct mapmodelinfo { 
    int rad, h, zoff; 
    string name; 
    Model* m; 
};

class ModelManager 
{
private:

public:
    ModelManager();
    ~ModelManager();

    void initilize();
    void shutdown();

    void addModel(const Model& mdl);
};

#endif