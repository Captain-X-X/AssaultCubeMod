#ifndef WATER_H
#define WATER_H
#include "cube.h"

void setwatercolor(const char *r = "", const char *g = "", const char *b = "", const char *a = "");
void calcwaterscissor();
void addwaterquad(int x, int y, int size);
int renderwater(float hf, GLuint reflecttex, GLuint refracttex);
void resetwater();

#endif