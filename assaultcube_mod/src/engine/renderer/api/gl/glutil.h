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

#endif