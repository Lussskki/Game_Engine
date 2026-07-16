#pragma once

namespace Engine
{

class Framebuffer
{
public:
    Framebuffer() = default;
    ~Framebuffer();

    bool Create(int width, int height);
    void Resize(int width, int height);
    void Bind() const;
    void Unbind() const;

    unsigned int GetColorTextureId() const
    {
        return m_ColorTextureId;
    }

    int GetWidth() const
    {
        return m_Width;
    }

    int GetHeight() const
    {
        return m_Height;
    }

private:
    void Destroy();

    unsigned int m_FramebufferId = 0;
    unsigned int m_ColorTextureId = 0;
    unsigned int m_DepthStencilRenderbufferId = 0;
    int m_Width = 0;
    int m_Height = 0;
};

}
