#pragma once

#include "Input/Input.h"
#include "Renderer/Camera.h"
#include "Renderer/Framebuffer.h"
#include "Renderer/Mesh.h"
#include "Renderer/Shader.h"
#include "Scene/Scene.h"

#include <memory>

namespace Engine
{

struct LightingSettings
{
    float AmbientStrength = 0.35f;
    float DirectionX = -0.4f;
    float DirectionY = -1.0f;
    float DirectionZ = -0.3f;
    float Intensity = 0.75f;
};

class Renderer
{
public:
    Renderer() = default;
    ~Renderer() = default;

    bool Initialize();
    void UpdateCamera(const Input& input, float deltaTime, bool allowMouseLook);
    void ZoomCamera(float amount);
    void PanCamera(float rightAmount, float upAmount);
    void BeginScreenFrame(int width, int height, float r, float g, float b, float a) const;
    void RenderSceneToViewport(const Scene& scene, int width, int height);

    unsigned int GetViewportTextureId() const;
    LightingSettings& GetLightingSettings();

private:
    void DrawScene(const Scene& scene, float aspectRatio) const;

    std::unique_ptr<Shader> m_Shader;
    std::unique_ptr<Mesh> m_CubeMesh;
    std::unique_ptr<Mesh> m_GridMesh;
    std::unique_ptr<Framebuffer> m_ViewportFramebuffer;
    Camera m_Camera;
    float m_CameraMoveSpeed = 4.0f;
    float m_MouseLookSpeed = 0.0035f;
    LightingSettings m_Lighting;
};

}






