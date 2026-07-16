#pragma once

#include "Scene/Scene.h"

struct GLFWwindow;

namespace Engine
{

class EditorGui
{
public:
    bool Initialize(GLFWwindow* window);
    void Shutdown();

    void BeginFrame();
    void Draw(Scene& scene, float deltaTime, unsigned int viewportTextureId);
    void EndFrame();

    bool WantsMouseCapture() const;
    bool WantsKeyboardCapture() const;
    bool WantsTextInput() const;

    bool IsViewportHovered() const
    {
        return m_ViewportHovered;
    }

    bool IsViewportFocused() const
    {
        return m_ViewportFocused;
    }

    int GetViewportWidth() const
    {
        return m_ViewportWidth;
    }

    int GetViewportHeight() const
    {
        return m_ViewportHeight;
    }

private:
    void DrawMenuBar(Scene& scene);
    void DrawViewport(unsigned int viewportTextureId);
    void DrawHierarchy(Scene& scene);
    void DrawInspector(Scene& scene);
    void DrawStats(float deltaTime);
    void DrawControls();

    bool m_Initialized = false;
    bool m_ViewportHovered = false;
    bool m_ViewportFocused = false;
    int m_ViewportWidth = 900;
    int m_ViewportHeight = 600;
};

}
