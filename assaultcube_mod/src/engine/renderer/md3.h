#ifndef CMEMD3MDL_H
#define CMEMD3MDL_H
#include "cube.h"
#include "vert_model.h"

// struct vertmodel;
struct modelattach;
class dynent;
struct md3 : vertmodel {
    md3(const char *name);
    ~md3() { }
    
    int type();
    
    void render(int anim, int varseed, float speed, int basetime, const vec3_t &o, float yaw, float pitch, dynent *d, modelattach *a, float scale);
    void rendershadow(int anim, int varseed, float speed, int basetime, const vec3_t &o, float yaw, modelattach *a);
    bool load();
};
extern md3 *loadingmd3;
// extern string md3dir;

void md3load(char *model);
void md3skin(char *objname, char *skin);
void md3anim(char *anim, int *startframe, int *range, float *speed);
void md3link(int *parent, int *child, char *tagname);
void md3emit(char *tag, int *type, int *arg1, int *arg2);

#endif