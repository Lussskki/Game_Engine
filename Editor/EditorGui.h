#pragma once

#include "Input/Input.h"
#include "Renderer/Renderer.h"
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

enum class EditorAxis
{
    None,
    X,
    Y,
    Z
};    


class EditorGui
{
public:
    bool Initialize(GLFWwindow* window);
    void Shutdown();

    void BeginFrame();
    void Draw(Scene& scene, Renderer& renderer, const Input& input, float deltaTime, unsigned int viewportTextureId);
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

    bool IsPlaying() const
    {
        return m_Playing;
    }
    float GetMouseWheel() const;

private:
    void DrawMenuBar(Scene& scene);
    void DrawViewport(Scene& scene, Renderer& renderer, unsigned int viewportTextureId);
    void DrawHierarchy(Scene& scene);
    void DrawInspector(Scene& scene);
    void DrawStats(float deltaTime);
    void DrawConsole();
    void DrawLighting(Renderer& renderer);
    void DrawControls();
    void DrawToolbar();
    void DrawProjectActionsMenu(Scene& scene);
    void HandleKeyboardShortcuts(Scene& scene, const Input& input);
    void HandleViewportMouse(Scene& scene, Renderer& renderer);


    EditorTool m_CurrentTool = EditorTool::Select;
    EditorAxis m_ActiveMoveAxis = EditorAxis::None;
    EditorAxis m_ActiveRotateAxis = EditorAxis::None;
    bool m_Playing = false;
    bool m_DraggingSelectedObject = false;
    Vec3 m_SelectedDragOffset;
    bool m_Initialized = false;
    bool m_ViewportHovered = false;
    bool m_ViewportFocused = false;
    bool m_EditingText = false;
    int m_ViewportWidth = 900;
    int m_ViewportHeight = 600;
    float m_ViewportImageMinX = 0.0f;
    float m_ViewportImageMinY = 0.0f;
    std::array<char, 128> m_ProjectNameBuffer = {};
    std::array<char, 128> m_SceneNameBuffer = {};
};

}














