#include "Renderer/VertexArray.h"

#include <GL/glew.h>

namespace Engine
{

VertexArray::~VertexArray()
{
    if (m_Id != 0)
    {
        glDeleteVertexArrays(1, &m_Id);
    }
}

void VertexArray::Create()
{
    glGenVertexArrays(1, &m_Id);
}

void VertexArray::Bind() const
{
    glBindVertexArray(m_Id);
}

void VertexArray::Unbind() const
{
    glBindVertexArray(0);
}

void VertexArray::AddFloatAttribute(unsigned int index, int count, int stride, const void* offset) const
{
    glEnableVertexAttribArray(index);
    glVertexAttribPointer(index, count, GL_FLOAT, GL_FALSE, stride, offset);
}

}
