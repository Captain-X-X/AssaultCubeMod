#ifndef MD2MDL_H
#define MD2MDL_H
#include "Model.h"

struct modelattach;
class dynent;

/*
* ==========
* MD2 model loader
* ==========
*/
class ModelMD2 : public Model 
{
public:
    ModelMD2(const char *name);
    ~ModelMD2();

    void rendershadow(int anim, int varseed, float speed, int basetime, const vec3_t &o, float yaw, modelattach *a);
    bool load();
};

void md2anim(char *anim, int *frame, int *range, float *speed);
void md2tag(char *name, char *vert1, char *vert2, char *vert3, char *vert4);
void md2emit(char *tag, int *type, int *arg1, int *arg2);

#endif