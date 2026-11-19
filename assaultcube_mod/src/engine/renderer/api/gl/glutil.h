#ifndef GLUTILS_H
#define GLUTILS_H

/*
* ==========
* opengl shader helpers
* ==========
*/
enum class GL_CompileErrorTypes_t : unsigned int {
    SHA_VERTEX,
    SHA_FRAGMENT,
    SHA_PROGRAM,
};
bool GL_GetCompileErrors(unsigned int shader, GL_CompileErrorTypes_t type);

unsigned int GL_CompileShader(const char* vertSrc, const char* fragSrc);
unsigned int GL_CreateTexture2D(unsigned char* pixles, int w, int h, unsigned int bpp, unsigned int minFilter, unsigned int magFilter, unsigned int wrapMode);
unsigned int GL_GetTextureFormatSize(unsigned int format);

#endif