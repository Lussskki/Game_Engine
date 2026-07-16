#include "Renderer/Renderer.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>

namespace Engine
{

bool Renderer::Initialize()
{
    glEnable(GL_DEPTH_TEST);

    m_CubeMesh = std::make_unique<Mesh>();
    if (!m_CubeMesh->CreateCube())
    {
        std::cerr << "Failed to create cube mesh" << std::endl;
        return false;
    }

    m_Shader = std::make_unique<Shader>();
    if (!m_Shader->LoadFromFiles("Shaders/basic.vert", "Shaders/basic.frag"))
    {
        std::cerr << "Failed to load cube shader" << std::endl;
        return false;
    }

    m_ViewportFramebuffer = std::make_unique<Framebuffer>();
    if (!m_ViewportFramebuffer->Create(1280, 720))
    {
        std::cerr << "Failed to create viewport framebuffer" << std::endl;
        return false;
    }

    return true;
}

void Renderer::UpdateCamera(const Input& input, float deltaTime)
{
    const float move = m_CameraMoveSpeed * deltaTime;

    if (input.IsKeyPressed(GLFW_KEY_W))
    {
        m_Camera.MoveForward(move);
    }

    if (input.IsKeyPressed(GLFW_KEY_S))
    {
        m_Camera.MoveForward(-move);
    }

    if (input.IsKeyPressed(GLFW_KEY_A))
    {
        m_Camera.MoveRight(-move);
    }

    if (input.IsKeyPressed(GLFW_KEY_D))
    {
        m_Camera.MoveRight(move);
    }

    if (input.IsKeyPressed(GLFW_KEY_SPACE))
    {
        m_Camera.MoveUp(move);
    }

    if (input.IsKeyPressed(GLFW_KEY_C))
    {
        m_Camera.MoveUp(-move);
    }

    if (input.IsMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT))
    {
        m_Camera.Rotate(
            static_cast<float>(input.GetMouseDeltaX()) * m_MouseLookSpeed,
            static_cast<float>(-input.GetMouseDeltaY()) * m_MouseLookSpeed
        );
    }
}

void Renderer::BeginScreenFrame(int width, int height, float r, float g, float b, float a) const
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, width, height);
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::RenderSceneToViewport(const Scene& scene, int width, int height)
{
    m_ViewportFramebuffer->Resize(width, height);
    m_ViewportFramebuffer->Bind();

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.06f, 0.07f, 0.085f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const float aspectRatio = height > 0 ? static_cast<float>(width) / static_cast<float>(height) : 1.0f;
    DrawScene(scene, aspectRatio);

    m_ViewportFramebuffer->Unbind();
}

unsigned int Renderer::GetViewportTextureId() const
{
    return m_ViewportFramebuffer->GetColorTextureId();
}

void Renderer::DrawScene(const Scene& scene, float aspectRatio) const
{
    const auto viewProjection = m_Camera.GetViewProjection(aspectRatio);

    m_Shader->Bind();
    m_Shader->SetMat4("u_ViewProjection", viewProjection);

    for (const SceneObject& object : scene.GetObjects())
    {
        m_Shader->SetMat4("u_Model", object.TransformData.GetMatrix());
        m_CubeMesh->Draw();
    }
}

}
