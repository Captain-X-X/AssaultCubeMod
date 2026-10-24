#ifndef HUDGUN_H
#define HUDGUN_H
#include "cube.h"

extern int nosway;
extern int swayspeeddiv;
extern int swaymovediv; 
extern int swayupspeeddiv;
extern int swayupmovediv; 

struct weaponmove
{
    static vec swaydir;
    static int swaymillis, lastsway;

    float k_rot, kick;
    vec pos;
    int anim, basetime;
    
    weaponmove();

    void calcmove(vec aimdir, int lastaction, playerent *p);
};

#endif