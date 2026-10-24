#ifndef ENTITIES_H
#define ENTITIES_H
#include "cube.h"

void spawnallitems();
void pickupeffects(int n, playerent *d);
void renderentities();
void rendermapmodels();
void resetspawns(int type = -1);
void setspawn(int i, bool on);
void checkitems(playerent *d);
extern vector<int> changedents;
void syncentchanges(bool force = false);

#endif