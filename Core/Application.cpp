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

        const bool guiWantsMouse = m_Gui.WantsMouseCapture();
        const bool guiWantsKeyboard = m_Gui.WantsKeyboardCapture();
        const bool cameraLookActive = m_Input.IsMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT) && !guiWantsMouse;

        m_Window->SetCursorCaptured(cameraLookActive);

        if (!guiWantsKeyboard || cameraLookActive)
        {
            m_Renderer->UpdateCamera(m_Input, deltaTime);
            m_Editor.Update(m_Scene, m_Input, deltaTime);
        }

        const float width = static_cast<float>(m_Window->GetWidth());
        const float height = static_cast<float>(m_Window->GetHeight());
        const float aspectRatio = height > 0.0f ? width / height : 1.0f;

        m_Renderer->BeginFrame(0.08f, 0.09f, 0.11f, 1.0f);
        m_Renderer->DrawScene(m_Scene, aspectRatio);

        m_Gui.Draw(m_Scene, deltaTime);
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
