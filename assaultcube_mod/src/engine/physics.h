#ifndef PHYSICS_H
#define PHYSICS_H
#include "cube.h"

float raycube(const vec &o, const vec &ray, vec &surface);
bool raycubelos(const vec &from, const vec &to, float margin = 0);
void moveplayer(physent *pl, int moveres, bool local);
void moveplayer(physent *pl, int moveres, bool local, int curtime);
void movebounceent(bounceent *p, int moveres, bool local);
void entinmap(physent *d);
void physicsframe();
void mousemove(int dx, int dy);
void fixcamerarange(physent *cam = camera1);
void updatecrouch(playerent *p, bool on);
bool objcollide(physent *d, const vec &objpos, float objrad, float objheight);
bool collide(physent *d, bool spawn = false, float drop = 0, float rise = 0, int level = 7);
void attack(bool on);
// void vecfromyawpitch(float yaw, float pitch, int move, int strafe, vec &m);
// void vectoyawpitch(const vec &v, float &yaw, float &pitch);

#endif