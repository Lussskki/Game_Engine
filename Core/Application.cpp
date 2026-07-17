#include "Core/Application.h"

#include <GLFW/glfw3.h>
#include <iostream>

namespace Engine
{

Application::Application() = default;
Application::~Application() = default;

bool Application::Initialize()
{
    m_Window = std::make_unique<Window>(1280, 720, "Renderer Engine");
    if (!m_Window->Initialize())
    {
        std::cerr << "Failed to initialize window" << std::endl;
        return false;
    }

    m_Renderer = std::make_unique<Renderer>();
    if (!m_Renderer->Initialize())
    {
        std::cerr << "Failed to initialize renderer" << std::endl;
        return false;
    }

    if (!m_Gui.Initialize(m_Window->GetNativeWindow()))
    {
        std::cerr << "Failed to initialize editor GUI" << std::endl;
        return false;
    }

    m_Running = true;
    return true;
}

void Application::Run()
{
    float lastTime = static_cast<float>(glfwGetTime());

    while (m_Running && !m_Window->ShouldClose())
    {
        const float currentTime = static_cast<float>(glfwGetTime());
        const float deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        m_Window->PollEvents();
        m_Input.Update(m_Window->GetNativeWindow());
        m_Gui.BeginFrame();

        const bool viewportHovered = m_Gui.IsViewportHovered();
        const bool cameraToolActive = m_Gui.IsCameraToolSelected();
        const bool cameraLookActive = viewportHovered && m_Input.IsMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT);
        const bool selectToolActive = m_Gui.IsSelectToolSelected();
        const bool moveToolActive = m_Gui.IsMoveToolSelected();
        const bool rotateToolActive = m_Gui.IsRotateToolSelected();
        const bool cameraPanActive = viewportHovered &&
            (m_Input.IsMouseButtonPressed(GLFW_MOUSE_BUTTON_MIDDLE) ||
             (selectToolActive && m_Input.IsMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT)));
        const bool textInputActive = m_Gui.WantsTextInput() || m_Gui.IsEditingText();
        const bool keyboardCaptured = m_Gui.WantsKeyboardCapture() || textInputActive;

        m_Window->SetCursorCaptured(cameraLookActive);

        if (viewportHovered)
        {
            m_Renderer->UpdateCamera(m_Input, deltaTime, cameraLookActive);

            if (cameraPanActive)
            {
                m_Renderer->PanCamera(
                    static_cast<float>(-m_Input.GetMouseDeltaX()) * 0.01f,
                    static_cast<float>(m_Input.GetMouseDeltaY()) * 0.01f
                );
            }

            const float wheel = m_Gui.GetMouseWheel();
            if (wheel != 0.0f && !moveToolActive && !rotateToolActive)
            {
                m_Renderer->ZoomCamera(wheel * 0.8f);
            }
        }

        if (!keyboardCaptured)
        {
            m_Editor.Update(m_Scene, m_Input, deltaTime, viewportHovered);
        }

        m_Renderer->RenderSceneToViewport(m_Scene, m_Gui.GetViewportWidth(), m_Gui.GetViewportHeight());
        m_Renderer->BeginScreenFrame(m_Window->GetWidth(), m_Window->GetHeight(), 0.035f, 0.038f, 0.045f, 1.0f);

        m_Gui.Draw(m_Scene, *m_Renderer, deltaTime, m_Renderer->GetViewportTextureId());
        m_Gui.EndFrame();

        m_Window->SwapBuffers();
    }
}

void Application::Shutdown()
{
    m_Gui.Shutdown();
    m_Renderer.reset();
    m_Window.reset();
    m_Running = false;
}

}













