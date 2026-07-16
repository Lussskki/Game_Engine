#include "Renderer/IndexBuffer.h"

#include <GL/glew.h>

namespace Engine
{

IndexBuffer::~IndexBuffer()
{
    if (m_Id != 0)
    {
        glDeleteBuffers(1, &m_Id);
    }
}

void IndexBuffer::Create(const unsigned int* data, unsigned int count)
{
    m_Count = count;
    glGenBuffers(1, &m_Id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, GL_STATIC_DRAW);
}

void IndexBuffer::Bind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Id);
}

void IndexBuffer::Unbind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

}
