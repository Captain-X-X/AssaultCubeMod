#include "pch.h"
#include "vertexarray.h"

VertexArray::VertexArray()
    : m_id(NULL)
{
    
}

VertexArray::~VertexArray()
{
    destroy();
}

void VertexArray::create()
{
    if (m_id != NULL) {
        printf("VertexArray already created!\n");
        return;
    }
    glGenVertexArrays(1, &m_id);
    glBindVertexArray(m_id);

    // printf("vertex array created: %d\n", m_id);
}
void VertexArray::destroy()
{
    if(m_id != NULL) {
        glDeleteVertexArrays(1, &m_id);
        m_id = NULL;
    }
}

void VertexArray::bind()
{
    glBindVertexArray(m_id);
}
void VertexArray::unbind()
{
    glBindVertexArray(0);
}

// getters
unsigned int VertexArray::getId() const
{
    return m_id;
}