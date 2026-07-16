#pragma once

namespace Engine
{

class VertexBuffer
{
public:
    VertexBuffer() = default;
    ~VertexBuffer();

    void Create(const void* data, unsigned int size);
    void Bind() const;
    void Unbind() const;

private:
    unsigned int m_Id = 0;
};

}
