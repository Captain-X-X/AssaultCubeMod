#ifndef CUBEVERTANIMATION_H
#define CUBEVERTANIMATION_H
#include "maths/vector_maths.h"

enum { 
    ANIM_IDLE = 0, 
    ANIM_RUN, 
    ANIM_ATTACK, 
    ANIM_PAIN, 
    ANIM_JUMP, 
    ANIM_LAND, 
    ANIM_FLIPOFF, 
    ANIM_SALUTE, 
    ANIM_TAUNT, 
    ANIM_WAVE, 
    ANIM_POINT, 
    ANIM_CROUCH_IDLE, 
    ANIM_CROUCH_WALK, 
    ANIM_CROUCH_ATTACK, 
    ANIM_CROUCH_PAIN, 
    ANIM_CROUCH_DEATH, 
    ANIM_DEATH, 
    ANIM_LYING_DEAD, 
    ANIM_FLAG, 
    ANIM_GUN_IDLE, 
    ANIM_GUN_SHOOT, 
    ANIM_GUN_RELOAD, 
    ANIM_GUN_THROW, 
    ANIM_MAPMODEL, 
    ANIM_TRIGGER, 
    ANIM_DECAY, 
    ANIM_ALL, 
    NUMANIMS 
};

#define ANIM_INDEX       0xFF
#define ANIM_LOOP        (1<<8)
#define ANIM_START       (1<<9)
#define ANIM_END         (1<<10)
#define ANIM_REVERSE     (1<<11)
#define ANIM_NOINTERP    (1<<12)
#define ANIM_MIRROR      (1<<13)
#define ANIM_NOSKIN      (1<<14)
#define ANIM_TRANSLUCENT (1<<15)
#define ANIM_PARTICLE    (1<<16)
#define ANIM_DYNALLOC    (1<<17)

struct animstate  // used for animation blending of animated characters
{
    int anim, frame, range, basetime;
    float speed;
    animstate() { reset(); }
    void reset() { anim = frame = range = basetime = 0; speed = 100.0f; };

    bool operator==(const animstate &o) const { return frame==o.frame && range==o.range && basetime==o.basetime && speed==o.speed; }
    bool operator!=(const animstate &o) const { return frame!=o.frame || range!=o.range || basetime!=o.basetime || speed!=o.speed; }
};

struct anpos
{
    int fr1, fr2;
    float t;

    void setframes(const animstate &as);
    bool operator==(const anpos &a) const { return fr1==a.fr1 && fr2==a.fr2 && (fr1==fr2 || t==a.t); }
    bool operator!=(const anpos &a) const { return fr1!=a.fr1 || fr2!=a.fr2 || (fr1!=fr2 && t!=a.t); }
};

struct animinfo
{
    int frame, range;
    float speed;
};

struct particleemitter
{
    int type, args[2], seed, lastemit;

    particleemitter() : type(-1), seed(-1), lastemit(-1)
    {
        memset(args, 0, sizeof(args));
    }
};

struct tag
{
    char *name;
    vec3_t pos;
    float transform[3][3];

    tag() : name(NULL) {}
    ~tag() 
    {
        if (name) {
            delete[] name;
            name = 0;
        } 
    }

    void identity()
    {
        transform[0][0] = 1;
        transform[0][1] = 0;
        transform[0][2] = 0;

        transform[1][0] = 0;
        transform[1][1] = 1;
        transform[1][2] = 0;

        transform[2][0] = 0;
        transform[2][1] = 0;
        transform[2][2] = 1;
    }
};

class part;
struct linkedpart
{
    part *p;
    particleemitter *emitter;
    vec3_t *pos;

    linkedpart() : p(NULL), emitter(NULL), pos(NULL) {}
    ~linkedpart() 
    {
        if (emitter) {
            delete[] emitter;
            emitter = 0;
        }  
    }
};

enum { 
    MDL_MD2 = 1, 
    MDL_MD3 
};

struct model;
struct modelattach
{
    const char *tag, *name;
    vec3_t *pos;
    model *m;

    modelattach() : tag(NULL), name(NULL), pos(NULL), m(NULL) {}
    modelattach(const char *tag, const char *name) : tag(tag), name(name), pos(NULL), m(NULL) {}
    modelattach(const char *tag, vec3_t *pos) : tag(tag), name(NULL), pos(pos), m(NULL) {}
};

#endif