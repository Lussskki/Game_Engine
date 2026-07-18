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
    void UpdatePlayMode(float deltaTime, bool textInputActive, bool cameraLookActive);
    int FindCharacterIndex() const;
    float FindGroundY(float x, float z, float fallbackY) const;
    float FindCharacterSupportY(const SceneObject& character, float x, float z, float fallbackY) const;
    bool IsBlockedByCollision(const SceneObject& character, const Vec3& candidatePosition) const;
    void ResolveCharacterCollisions(SceneObject& character);
    float m_CharacterVerticalVelocity = 0.0f;
    bool m_CharacterGrounded = false;
    std::unique_ptr<Window> m_Window;
    std::unique_ptr<Renderer> m_Renderer;
    Scene m_Scene;
    Input m_Input;
    EditorController m_Editor;
    EditorGui m_Gui;
    bool m_Running = false;
};

}
