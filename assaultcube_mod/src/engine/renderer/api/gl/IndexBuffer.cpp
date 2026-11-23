#include "IndexBuffer.h"

IndexBuffer::IndexBuffer()
    : m_id(NULL), m_size(NULL), m_pIndexDataMap(nullptr)
{

}

IndexBuffer::~IndexBuffer()
{
    destroy();
}

void IndexBuffer::create(unsigned int size)
{
    if (m_id != NULL) {
        printf("IndexBuffer already created!\n");
        return;
    }
    m_size = size;
    glGenBuffers(1, &m_id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size * sizeof(unsigned int), 0, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void IndexBuffer::destroy()
{
    if (m_id != NULL) {
        glDeleteBuffers(1, &m_id);
        m_id = 0;
    }
}

void IndexBuffer::bind()
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id);
}

void IndexBuffer::unbind()
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void* IndexBuffer::mapBuffer()
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id);
    m_pIndexDataMap = glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY);
    return m_pIndexDataMap;
}

void IndexBuffer::addData(unsigned int* indexArray, unsigned int arraySize)
{
    if (m_pIndexDataMap == nullptr){
        printf("failed to write data to index buffer: %d. buffer not mapped!\n", m_id);
        return;
    }
    for (unsigned int i = 0; i < arraySize; i++) {
        static_cast<unsigned int*>(m_pIndexDataMap)[i] = indexArray[i];
    }
}

void IndexBuffer::addData(std::vector<unsigned int>& indexArray)
{
    addData(indexArray.data(), indexArray.size());
}

void IndexBuffer::unmapBuffer()
{
    glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

// getters
unsigned int IndexBuffer::getId() const
{
    return m_id;
}
