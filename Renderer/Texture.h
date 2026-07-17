#pragma once

#include <string>

namespace Engine
{

class Texture
{
public:
    Texture() = default;
    ~Texture();

    bool CreateCheckerboard();
    bool LoadFromFile(const std::string& path);
    void Bind(unsigned int slot = 0) const;

    unsigned int GetId() const
    {
        return m_Id;
    }

    bool IsValid() const
    {
        return m_Id != 0;
    }

private:
    void Destroy();

    unsigned int m_Id = 0;
    int m_Width = 0;
    int m_Height = 0;
};

}
