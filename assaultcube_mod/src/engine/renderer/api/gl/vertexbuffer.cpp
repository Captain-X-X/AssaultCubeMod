#include "vertexbuffer.h"

VertexBuffer::VertexBuffer()
    : m_id(NULL), m_size(NULL), m_pDataMap(nullptr)
{
    
}

VertexBuffer::~VertexBuffer()
{
    destroy();
}

void VertexBuffer::create(unsigned int size, VertexBufferLayoutType_t defaultLayout)
{
    if (m_id != NULL) {
        printf("VertexBuffer already created!\n");
        return;
    }
    m_size = size;

    glGenBuffers(1, &m_id);
    glBindBuffer(GL_ARRAY_BUFFER, m_id);
    glBufferData(GL_ARRAY_BUFFER, size, 0, GL_DYNAMIC_DRAW);

    m_layout = defaultLayout;
    switch (defaultLayout) {
    case VertexBufferLayoutType_t::DEF_2D: {
        setUniformLocation(0, 3, STRIDE(VertexData2D), GL_FLOAT, (const void*)0);
        setUniformLocation(1, 4, STRIDE(VertexData2D), GL_FLOAT, (const void*)(offsetof(VertexData2D, VertexData2D::colour)));
        setUniformLocation(2, 2, STRIDE(VertexData2D), GL_FLOAT, (const void*)(offsetof(VertexData2D, VertexData2D::uv)));
        setUniformLocation(3, 1, STRIDE(VertexData2D), GL_UNSIGNED_INT, (const void*)(offsetof(VertexData2D, VertexData2D::texture)));
        break;
    }
    case VertexBufferLayoutType_t::DEF_3D: {
        setUniformLocation(0, 3, STRIDE(VertexData2D), GL_FLOAT, (const void*)0);
        setUniformLocation(1, 3, STRIDE(VertexData2D), GL_FLOAT, (const void*)(offsetof(VertexData3D, VertexData3D::normal)));
        setUniformLocation(2, 4, STRIDE(VertexData2D), GL_FLOAT, (const void*)(offsetof(VertexData3D, VertexData3D::colour)));
        setUniformLocation(3, 2, STRIDE(VertexData2D), GL_FLOAT, (const void*)(offsetof(VertexData3D, VertexData3D::uv)));
        setUniformLocation(4, 1, STRIDE(VertexData2D), GL_UNSIGNED_INT, (const void*)(offsetof(VertexData3D, VertexData3D::texture)));
        break;
    }
    default:{ break; }
    }

    // printf("vertex buffer created: %d\n", m_id);
}

void VertexBuffer::setUniformLocation(unsigned int location, unsigned int size, unsigned int stride, unsigned int gltype, const void* pData)
{
    glVertexAttribPointer(location, size, gltype, false, stride, pData);
    glEnableVertexAttribArray(location);
}

void VertexBuffer::submitData2D(std::vector<VertexData2D>& data)
{
    if (m_id == NULL){
        return;
    }
    if (m_layout != VertexBufferLayoutType_t::DEF_2D){
        return;
    }
    mapBuffer();
    VertexData2D* pData = (VertexData2D*)m_pDataMap;    
    for (size_t i = 0; i < data.size(); i++) {
        pData->position = data[i].position;
        pData->colour = data[i].colour;
        pData->uv = data[i].uv;
        pData->texture = data[i].texture;
        pData++;
    }
    unmapBuffer();
}

void VertexBuffer::submitData3D(std::vector<VertexData3D>& data)
{
    if (m_id == NULL){
        return;
    }
    if (m_layout != VertexBufferLayoutType_t::DEF_3D){
        return;
    }
    mapBuffer();
    VertexData3D* pData = (VertexData3D*)m_pDataMap;    
    for (size_t i = 0; i < data.size(); i++) {
        pData->position = data[i].position;
        pData->normal = data[i].normal;
        pData->colour = data[i].colour;
        pData->uv = data[i].uv;
        pData->texture = data[i].texture;
        pData++;
    }
    unmapBuffer();
}

void VertexBuffer::destroy()
{
    if(m_id != NULL) {
        glDeleteBuffers(1, &m_id);
        m_id = NULL;
    }
}

void VertexBuffer::bind()
{
    glBindBuffer(GL_ARRAY_BUFFER, m_id);
}

void VertexBuffer::unbind()
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void* VertexBuffer::mapBuffer()
{
    bind();
    this->m_pDataMap = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
}

void VertexBuffer::unmapBuffer()
{
    glUnmapBuffer(GL_ARRAY_BUFFER);
    glBindBuffer(GL_ARRAY_BUFFER, m_id);
}

unsigned int VertexBuffer::getId() const
{
    return m_id;
}