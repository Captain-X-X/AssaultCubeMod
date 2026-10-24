#ifndef EDITING_H
#define EDITING_H
#include "cube.h"

void cursorupdate();
void toggleedit(bool force = false);
char *editinfo();
void editdrag(bool isdown);
void checkselections();
void setvdeltaxy(int delta, block &sel);
void editequalisexy(bool isfloor, block &sel);
void edittypexy(int type, block &sel);
void edittexxy(int type, int t, block &sel);
void editheightxy(bool isfloor, int amount, block &sel);
// bool noteditmode();
bool noteditmode(const char* func = NULL);
void pruneundos(int maxremain = 0);

#endif