#ifndef GLVERTEXBUFFER_H
#define GLVERTEXBUFFER_H

#include "pch.h"
#include "../../types/VertexData.h"

// #define STRIDE(a, b) a * sizeof(b)
#define STRIDE(b) sizeof(b)

enum class VertexBufferLayoutType_t : unsigned int {
    NONE,
    DEF_2D,
    DEF_3D,
};

class VertexBuffer
{
private:
    unsigned int m_id;
    unsigned int m_size;
    void* m_pDataMap;
    VertexBufferLayoutType_t m_layout;

public:
    VertexBuffer();
    ~VertexBuffer();

    void create(unsigned int size, VertexBufferLayoutType_t defaultLayout = VertexBufferLayoutType_t::NONE);
    void setUniformLocation(unsigned int location, unsigned int size, unsigned int stride, unsigned int gltype, const void* pData = (void*)0);
    void submitData2D(std::vector<VertexData2D>& data);
    void submitData3D(std::vector<VertexData3D>& data);
    void destroy();

    void bind();
    void unbind();
    void* mapBuffer();
    void unmapBuffer();

    template<typename t>
    t mapBufferA() { return (t)mapBuffer(); }

    // getters
    unsigned int getId() const;
};

#endif 