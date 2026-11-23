#ifndef GLINDEXBUFFER_H
#define GLINDEXBUFFER_H

#include "pch.h"

class IndexBuffer
{
private:
    unsigned int m_id;
    unsigned int m_size;
    void* m_pIndexDataMap;

public:
    IndexBuffer();
    ~IndexBuffer();

    void create(unsigned int size);
    void destroy();

    void bind();
    void unbind();
    void* mapBuffer();
    void addData(unsigned int* indexArray, unsigned int arraySize);
    void addData(std::vector<unsigned int>& indis);
    void unmapBuffer();

    // getters
    unsigned int getId() const;
};

#endif