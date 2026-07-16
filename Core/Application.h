#pragma once

#include "Editor/EditorController.h"
#include "Editor/EditorGui.h"
#include "Input/Input.h"
#include "Platform/Window.h"
#include "Renderer/Renderer.h"
#include "Scene/Scene.h"

#include <memory>

namespace Engine
{

class Application
{
public:
    Application();
    ~Application();

    bool Initialize();
    void Run();
    void Shutdown();

private:
    std::unique_ptr<Window> m_Window;
    std::unique_ptr<Renderer> m_Renderer;
    Scene m_Scene;
    Input m_Input;
    EditorController m_Editor;
    EditorGui m_Gui;
    bool m_Running = false;
};

}
