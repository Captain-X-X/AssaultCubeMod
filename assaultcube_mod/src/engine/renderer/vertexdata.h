#ifndef RVERTEXDATA_H
#define RVERTEXDATA_H
#include "pch.h"
#include "texture.h"

struct VertexData2D
{
    vec3_t position;
    vec4_t colour;
    vec2_t uv;
    unsigned int texture;
};

struct VertexData3D
{
    vec3_t position;
    vec3_t normal;
    vec4_t colour;
    vec2_t uv;
    unsigned int texture;
};

#endif