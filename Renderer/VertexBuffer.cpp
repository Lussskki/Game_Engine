#include "Renderer/VertexBuffer.h"

#include <GL/glew.h>

namespace Engine
{

VertexBuffer::~VertexBuffer()
{
    if (m_Id != 0)
    {
        glDeleteBuffers(1, &m_Id);
    }
}

void VertexBuffer::Create(const void* data, unsigned int size)
{
    glGenBuffers(1, &m_Id);
    glBindBuffer(GL_ARRAY_BUFFER, m_Id);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

void VertexBuffer::Bind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, m_Id);
}

void VertexBuffer::Unbind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

}
