#include "Renderer/Framebuffer.h"

#include <GL/glew.h>
#include <iostream>

namespace Engine
{

Framebuffer::~Framebuffer()
{
    Destroy();
}

bool Framebuffer::Create(int width, int height)
{
    if (width <= 0 || height <= 0)
    {
        return false;
    }

    m_Width = width;
    m_Height = height;

    glGenFramebuffers(1, &m_FramebufferId);
    glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferId);

    glGenTextures(1, &m_ColorTextureId);
    glBindTexture(GL_TEXTURE_2D, m_ColorTextureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorTextureId, 0);

    glGenRenderbuffers(1, &m_DepthStencilRenderbufferId);
    glBindRenderbuffer(GL_RENDERBUFFER, m_DepthStencilRenderbufferId);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_Width, m_Height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_DepthStencilRenderbufferId);

    const bool complete = glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
    if (!complete)
    {
        std::cerr << "Framebuffer is incomplete" << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return complete;
}

void Framebuffer::Resize(int width, int height)
{
    if (width <= 0 || height <= 0)
    {
        return;
    }

    if (width == m_Width && height == m_Height && m_FramebufferId != 0)
    {
        return;
    }

    Destroy();
    Create(width, height);
}

void Framebuffer::Bind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferId);
    glViewport(0, 0, m_Width, m_Height);
}

void Framebuffer::Unbind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::Destroy()
{
    if (m_DepthStencilRenderbufferId != 0)
    {
        glDeleteRenderbuffers(1, &m_DepthStencilRenderbufferId);
        m_DepthStencilRenderbufferId = 0;
    }

    if (m_ColorTextureId != 0)
    {
        glDeleteTextures(1, &m_ColorTextureId);
        m_ColorTextureId = 0;
    }

    if (m_FramebufferId != 0)
    {
        glDeleteFramebuffers(1, &m_FramebufferId);
        m_FramebufferId = 0;
    }
}

}
