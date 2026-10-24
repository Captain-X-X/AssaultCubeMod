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

// /*
// */
// #include "mesh.h"

// class Model
// {
// protected:
//     std::vector<Mesh> m_meshs;

//     // max of all mesh combined
//     unsigned int m_vertexCount;
//     unsigned int m_indexCount;

// public:
//     Model();
//     virtual ~Model();

//     bool addMesh(Mesh& mesh);

//     std::vector<Mesh>& getMesh();
//     const std::vector<Mesh>& getMesh() const;
    
//     Mesh* getMesh(unsigned int index);
//     const Mesh* getMesh(unsigned int index) const;
// };

// //
// typedef class iModelHandle;

// class ModelManager
// {
// public:
//     struct modelInfo_t {
//         Model mdl;
//         unsigned int id;
//         const char* name;
//     };

// private:
//     std::vector<modelInfo_t> models;

// public:
//     ModelManager();
//     ~ModelManager();

//     void addModel(const Model& mdl, const char* name);
//     void addModel(modelInfo_t mdlInfo);

//     // create a VBO+VAO & IBO fore this entire model. all in one
//     void registerModelGLBuffers(Model& mdlReg);

//     static Model* GetModelFromHandle(iModelHandle& handle);
//     static unsigned int GetIDFromHandle(iModelHandle& handle);
// };

// namespace AssetImport
// {
//     Model LoadMD2Model(const char* modelFilePath, const mat4_t& loadMatrix);
//     Model LoadMD3Model(const char* modelFilePath, const mat4_t& loadMatrix);
// }

#endif