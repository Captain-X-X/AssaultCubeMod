#ifndef WORLDOCULL_H
#define WORLDOCULL_H
#include "cube.h"

void disableraytable();
void computeraytable(float vx, float vy, float fov);
int isoccluded(float vx, float vy, float cx, float cy, float csize);

#endif