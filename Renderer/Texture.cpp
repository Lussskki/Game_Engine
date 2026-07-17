#include "Renderer/Texture.h"

#include "Core/ConsoleLog.h"

#include <GL/glew.h>
#include <fstream>
#include <vector>

namespace Engine
{

Texture::~Texture()
{
    Destroy();
}

bool Texture::CreateCheckerboard()
{
    Destroy();

    m_Width = 2;
    m_Height = 2;
    const unsigned char pixels[] = {
        255, 255, 255, 255,   40,  40,  40, 255,
         40,  40,  40, 255,  255, 255, 255, 255
    };

    glGenTextures(1, &m_Id);
    glBindTexture(GL_TEXTURE_2D, m_Id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    return true;
}

bool Texture::LoadFromFile(const std::string& path)
{
    ConsoleLog::Warning("Texture loading needs stb_image added. Could not load: " + path);
    return false;
}

void Texture::Bind(unsigned int slot) const
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, m_Id);
}

void Texture::Destroy()
{
    if (m_Id != 0)
    {
        glDeleteTextures(1, &m_Id);
        m_Id = 0;
    }
}

}
