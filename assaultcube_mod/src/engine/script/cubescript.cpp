#include "pch.h"
#include "cube.h"
#include "cubescript.h"

/*
* ==========
* RENDERER SCRIPT EXPORT
* ==========
*/
bool hasext(const char *exts, const char *ext);
void glext(char *ext) 
{
    const char *exts = (const char *)glGetString(GL_EXTENSIONS);
    intret(hasext(exts, ext) ? 1 : 0);
}
COMMAND(glext, "s");

// void fovcompat(int *oldfov);
// COMMAND(fovcompat, "i");