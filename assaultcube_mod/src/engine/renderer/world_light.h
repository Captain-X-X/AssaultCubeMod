#ifndef WORLDLIGHT_H
#define WORLDLIGHT_H
#include "cube.h"

extern int lastcalclight;

void fullbrightlight(int level = -1);
void calclight();
void adddynlight(physent *owner, const vec &o, int reach, int expire, int fade, uchar r, uchar g = 0, uchar b = 0);
void dodynlights();
void undodynlights();
void cleardynlights();
void removedynlights(physent *owner);
block *blockcopy(const block &b);
void blockpaste(const block &b, int bx, int by, bool light);
void blockpaste(const block &b);
void freeblock(block *&b);

#endif