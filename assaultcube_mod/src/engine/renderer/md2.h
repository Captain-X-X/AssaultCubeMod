#ifndef CMEMD2MDL_H
#define CMEMD2MDL_H
#include "cube.h"
#include "model.h"

struct modelattach;
class dynent;

/*
* ==========
* MD2 model loader
* ==========
*/
class md2 : public model 
{
public:
    md2(const char *name);
    ~md2();

    void rendershadow(int anim, int varseed, float speed, int basetime, const vec3_t &o, float yaw, modelattach *a);
    bool load();
};

void md2anim(char *anim, int *frame, int *range, float *speed);
void md2tag(char *name, char *vert1, char *vert2, char *vert3, char *vert4);
void md2emit(char *tag, int *type, int *arg1, int *arg2);

#endif