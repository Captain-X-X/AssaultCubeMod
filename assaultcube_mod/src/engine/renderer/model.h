#ifndef MODEL_H
#define MODEL_H
#include "cube.h"

struct vertmodel;
struct modelattach;
class dynent;
struct model
{
    bool cullface, vertexlight, alphablend;  //ALX Alpha channel models
    float alphatest, translucency, scale, radius, shadowdist;
    vec3_t translate;
    int cachelimit, batch;
    
    //model() : cullface(true), vertexlight(false), alphatest(0.9f), translucency(0.25f), scale(1), radius(0), shadowdist(0), translate(0, 0, 0), cachelimit(8), batch(-1) {}
    model() : cullface(true), vertexlight(false),  alphablend(false), alphatest(0.9f), translucency(0.25f), scale(1), radius(0), shadowdist(0), translate(0, 0, 0), cachelimit(8), batch(-1) {}
    virtual ~model() {}

    virtual bool load() = 0;
    virtual char *name() = 0;
    virtual int type() = 0;

    virtual void cleanup() = 0;

    virtual void render(int anim, int varseed, float speed, int basetime, const vec3_t &o, float yaw, float pitch, dynent *d, modelattach *a = NULL, float scale = 1.0f) = 0;
    virtual void setskin(int tex = 0) = 0;

    virtual void genshadows(float height, float rad) {}
    virtual void rendershadow(int anim, int varseed, float speed, int basetime, const vec3_t &o, float yaw, modelattach *a = NULL) {}
    virtual bool hasshadows() { return false; }

    virtual void startrender() {}
    virtual void endrender() {}
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