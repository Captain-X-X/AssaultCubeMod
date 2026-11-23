#ifndef RVERTEXDATA_H
#define RVERTEXDATA_H
#include "pch.h"

struct VertexData2D
{
    Vector3 position;
    Vector4 colour;
    Vector2 uv;
};

struct VertexData3D
{
    Vector3 position;
    Vector3 normal;
    Vector4 colour;
    Vector2 uv;
    // unsigned int texture;
};

#endif