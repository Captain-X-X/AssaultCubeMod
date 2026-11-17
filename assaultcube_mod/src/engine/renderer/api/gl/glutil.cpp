#include "pch.h"
#include "glutil.h"

bool GL_GetCompileErrors(unsigned int shader, GL_CompileErrorTypes_t type)
{
    GLint hasCompiled;
    char infoLog[1024];
    if (type != GL_CompileErrorTypes_t::SHA_PROGRAM) {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &hasCompiled);
        if (hasCompiled == GL_FALSE) {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            printf("shader compile error for: %s. info_log:\n %s", 
                (type == GL_CompileErrorTypes_t::SHA_FRAGMENT) ? "frag" : "vert", 
                infoLog
            );
            return false;
        }
    }
    else {
        glGetProgramiv(shader, GL_LINK_STATUS, &hasCompiled);
        if (hasCompiled == GL_FALSE) {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            printf("shader program error: \n %s", type, infoLog);
            return false;
        }
    }
    return true;
}

unsigned int GL_CompileShader(const char* vertSrc, const char* fragSrc)
{
    bool bCreationSuccess = true;
    unsigned int shaderId = glCreateProgram();

    if (fragSrc != NULL) {
        unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragSrc, NULL);
        glCompileShader(fragmentShader);
        if(!GL_GetCompileErrors(fragmentShader, GL_CompileErrorTypes_t::SHA_FRAGMENT)) {
            return NULL;
        }
        glAttachShader(shaderId, fragmentShader);
        glDeleteShader(fragmentShader);
    }
    else{
        bCreationSuccess = false;
    }
    
    if (vertSrc != NULL) {
        unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertSrc, NULL);
        glCompileShader(vertexShader);
        if(!GL_GetCompileErrors(vertexShader, GL_CompileErrorTypes_t::SHA_VERTEX)) {
            return NULL;
        }
        glAttachShader(shaderId, vertexShader);
        glDeleteShader(vertexShader);
    }
    else{
        bCreationSuccess = false;
    }
    
    if (bCreationSuccess == false) {
        glDeleteProgram(shaderId);
        return NULL;
    }

    glLinkProgram(shaderId);
    // printf("Shader Created: %d\n", shaderId);
    return shaderId;
}
