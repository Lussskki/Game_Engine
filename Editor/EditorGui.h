#pragma once

#include "Scene/Scene.h"

#include <array>

struct GLFWwindow;

namespace Engine
{
enum class EditorTool
{
    Select,
    Move,
    Rotate,
    Scale,
    Camera
};    


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
    bool IsEditingText() const;

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

    bool IsCameraToolSelected() const
    {
        return m_CurrentTool == EditorTool::Camera;
    }

    bool IsSelectToolSelected() const
    {
        return m_CurrentTool == EditorTool::Select;
    }

    bool IsMoveToolSelected() const
    {
        return m_CurrentTool == EditorTool::Move;
    }

    bool IsRotateToolSelected() const
    {
        return m_CurrentTool == EditorTool::Rotate;
    }

    float GetMouseWheel() const;

private:
    void DrawMenuBar(Scene& scene);
    void DrawViewport(unsigned int viewportTextureId);
    void DrawHierarchy(Scene& scene);
    void DrawInspector(Scene& scene);
    void DrawStats(float deltaTime);
    void DrawControls();
    void DrawToolbar();
    void HandleViewportMouse(Scene& scene);


    EditorTool m_CurrentTool = EditorTool::Select;
    bool m_Initialized = false;
    bool m_ViewportHovered = false;
    bool m_ViewportFocused = false;
    bool m_EditingText = false;
    int m_ViewportWidth = 900;
    int m_ViewportHeight = 600;
    int m_NameEditIndex = -1;
    std::array<char, 128> m_NameBuffer = {};
};

}







