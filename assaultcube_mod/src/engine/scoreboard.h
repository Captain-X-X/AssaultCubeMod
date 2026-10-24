#ifndef SCOREBOARD_H
#define SCOREBOARD_H
#include "cube.h"

struct discscore { 
    int team, 
        flags, 
        frags, 
        deaths, 
        points; 

    char name[MAXNAMELEN + 1]; 
};

extern vector<discscore> discscores;

void showscores(bool on);
void renderscores(void *menu, bool init);
const char *asciiscores(bool destjpg = false);
void consolescores();
void teamflagscores(int &team1, int &team2);

#endif