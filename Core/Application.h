#pragma once

#include "Editor/EditorController.h"
#include "Editor/EditorGui.h"
#include "Input/Input.h"
#include "Platform/Window.h"
#include "Renderer/Renderer.h"
#include "Scene/Scene.h"

#include <memory>
#include <string>

namespace Engine
{

class Application
{
public:
    Application();
    ~Application();

    bool Initialize(bool playOnly = false, const std::string& scenePath = "");
    void Run();
    void Shutdown();

private:
    void UpdatePlayMode(float deltaTime, bool textInputActive, bool cameraLookActive);
    void UpdatePlayCamera();
    int FindCharacterIndex() const;
    float FindGroundY(float x, float z, float fallbackY) const;
    float FindCharacterSupportY(const SceneObject& character, float x, float z, float fallbackY) const;
    bool IsBlockedByCollision(const SceneObject& character, const Vec3& candidatePosition) const;
    void ResolveCharacterCollisions(SceneObject& character);
    float m_CharacterVerticalVelocity = 0.0f;
    bool m_CharacterGrounded = false;
    bool m_JumpQueued = false;
    std::unique_ptr<Window> m_Window;
    std::unique_ptr<Renderer> m_Renderer;
    Scene m_Scene;
    Input m_Input;
    EditorController m_Editor;
    EditorGui m_Gui;
    bool m_Running = false;
    bool m_PlayOnly = false;
    float m_PlayCameraYaw = 0.0f;
    float m_PlayCameraPitch = 0.25f;
    float m_PlayCameraDistance = 4.0f;
};

}
