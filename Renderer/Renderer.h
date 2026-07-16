#pragma once

#include "Input/Input.h"
#include "Renderer/Camera.h"
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
    void BeginFrame(float r, float g, float b, float a) const;
    void DrawScene(const Scene& scene, float aspectRatio) const;

private:
    std::unique_ptr<Shader> m_Shader;
    std::unique_ptr<Mesh> m_CubeMesh;
    Camera m_Camera;
    float m_CameraMoveSpeed = 4.0f;
    float m_MouseLookSpeed = 0.0035f;
};

}
