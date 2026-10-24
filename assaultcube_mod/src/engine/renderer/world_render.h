#ifndef WORLDRENDER_H
#define WORLDRENDER_H
#include "cube.h"

void render_world(float vx, float vy, float vh, float changelod, int yaw, int pitch, float fov, float fovy, int w, int h);
int lod_factor();

#endif