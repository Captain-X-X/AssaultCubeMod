#ifndef SHADOW_H
#define SHADOW_H
#include "cube.h"

bool addshadowbox(const vec &bbmin, const vec &bbmax, const vec &extrude, const glmatrixf &mat);
void drawstencilshadows();

#endif