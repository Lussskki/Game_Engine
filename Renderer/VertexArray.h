#pragma once

namespace Engine
{

class VertexArray
{
public:
    VertexArray() = default;
    ~VertexArray();

    void Create();
    void Bind() const;
    void Unbind() const;
    void AddFloatAttribute(unsigned int index, int count, int stride, const void* offset) const;

private:
    unsigned int m_Id = 0;
};

}
