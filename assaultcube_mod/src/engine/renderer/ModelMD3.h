#ifndef MD3MDL_H
#define MD3MDL_H
#include "Model.h"

struct modelattach;
class dynent;

/*
* ==========
* MD3 model loader
* ==========
*/
class ModelMD3 : public Model 
{
public:
    ModelMD3(const char *name);
    ~ModelMD3();
    
    void rendershadow(int anim, int varseed, float speed, int basetime, const vec3_t &o, float yaw, modelattach *a);
    bool load();
};
extern ModelMD3* loadingmd3;

void md3load(char *model);
void md3skin(char *objname, char *skin);
void md3anim(char *anim, int *startframe, int *range, float *speed);
void md3link(int *parent, int *child, char *tagname);
void md3emit(char *tag, int *type, int *arg1, int *arg2);

#endif