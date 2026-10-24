#ifndef DOC_H
#define DOC_H
#include "cube.h"

void renderdoc(int x, int y, int doch);
void renderdocmenu(void *menu, bool init);
void toggledoc();
void scrolldoc(int i);
int stringsort(const char **a, const char **b);

#endif