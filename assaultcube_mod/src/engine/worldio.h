#ifndef WORLDIO_H
#define WORLDIO_H
#include "cube.h"

extern int mapdims[8];
const char *setnames(const char *name);
void save_world(char *fname);
bool load_world(char *mname);
void writemap(char *name, int size, uchar *data);
void writecfggz(char *name, int size, int sizegz, uchar *data);
uchar *readmap(char *name, int *size, int *revision);
uchar *readmcfggz(char *name, int *size, int *sizegz);

#endif