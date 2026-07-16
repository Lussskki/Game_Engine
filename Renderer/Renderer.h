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

class Renderer
{
public:
    Renderer() = default;
    ~Renderer() = default;

    bool Initialize();
    void UpdateCamera(const Input& input, float deltaTime);
    void BeginScreenFrame(int width, int height, float r, float g, float b, float a) const;
    void RenderSceneToViewport(const Scene& scene, int width, int height);

    unsigned int GetViewportTextureId() const;

private:
    void DrawScene(const Scene& scene, float aspectRatio) const;

    std::unique_ptr<Shader> m_Shader;
    std::unique_ptr<Mesh> m_CubeMesh;
    std::unique_ptr<Framebuffer> m_ViewportFramebuffer;
    Camera m_Camera;
    float m_CameraMoveSpeed = 4.0f;
    float m_MouseLookSpeed = 0.0035f;
};

}
