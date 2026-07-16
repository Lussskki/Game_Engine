#pragma once

namespace Engine
{

class IndexBuffer
{
public:
    IndexBuffer() = default;
    ~IndexBuffer();

    void Create(const unsigned int* data, unsigned int count);
    void Bind() const;
    void Unbind() const;

    unsigned int GetCount() const
    {
        return m_Count;
    }

private:
    unsigned int m_Id = 0;
    unsigned int m_Count = 0;
};

}
