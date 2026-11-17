#ifndef GLVERTEXARRAY_H
#define GLVERTEXARRAY_H

#include "pch.h"

class VertexArray
{
private:
    unsigned int m_id;

public:
    VertexArray();
    ~VertexArray();

    void create();
    void destroy();

    void bind();
    void unbind();

    // getters
    unsigned int getId() const;
};

#endif