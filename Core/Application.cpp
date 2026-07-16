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

        if (m_Input.IsKeyPressed(GLFW_KEY_ESCAPE))
        {
            m_Window->RequestClose();
        }

        const bool viewportHovered = m_Gui.IsViewportHovered();
        const bool cameraLookActive = viewportHovered && m_Input.IsMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT);
        const bool textInputActive = m_Gui.WantsTextInput();

        m_Window->SetCursorCaptured(cameraLookActive);

        if (viewportHovered || cameraLookActive)
        {
            m_Renderer->UpdateCamera(m_Input, deltaTime);
        }

        if (!textInputActive)
        {
            m_Editor.Update(m_Scene, m_Input, deltaTime, viewportHovered);
        }

        m_Renderer->RenderSceneToViewport(m_Scene, m_Gui.GetViewportWidth(), m_Gui.GetViewportHeight());
        m_Renderer->BeginScreenFrame(m_Window->GetWidth(), m_Window->GetHeight(), 0.035f, 0.038f, 0.045f, 1.0f);

        m_Gui.Draw(m_Scene, deltaTime, m_Renderer->GetViewportTextureId());
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

