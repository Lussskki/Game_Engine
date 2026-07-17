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

    m_GridMesh = std::make_unique<Mesh>();
    if (!m_GridMesh->CreateGrid(20, 1.0f, -0.72f))
    {
        std::cerr << "Failed to create editor grid" << std::endl;
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

void Renderer::UpdateCamera(const Input& input, float deltaTime, bool allowMouseLook)
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

    if (allowMouseLook)
    {
        m_Camera.Rotate(
            static_cast<float>(input.GetMouseDeltaX()) * m_MouseLookSpeed,
            static_cast<float>(-input.GetMouseDeltaY()) * m_MouseLookSpeed
        );
    }
}

void Renderer::ZoomCamera(float amount)
{
    m_Camera.MoveForward(amount);
}

void Renderer::PanCamera(float rightAmount, float upAmount)
{
    m_Camera.MoveRight(rightAmount);
    m_Camera.MoveUp(upAmount);
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
    glClearColor(0.50f, 0.62f, 0.74f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const float aspectRatio = height > 0 ? static_cast<float>(width) / static_cast<float>(height) : 1.0f;
    DrawScene(scene, aspectRatio);

    m_ViewportFramebuffer->Unbind();
}

unsigned int Renderer::GetViewportTextureId() const
{
    return m_ViewportFramebuffer->GetColorTextureId();
}

LightingSettings& Renderer::GetLightingSettings()
{
    return m_Lighting;
}

void Renderer::DrawScene(const Scene& scene, float aspectRatio) const
{
    const auto viewProjection = m_Camera.GetViewProjection(aspectRatio);

    m_Shader->Bind();
    m_Shader->SetMat4("u_ViewProjection", viewProjection);
    m_Shader->SetInt("u_UseSolidColor", 0);
    m_Shader->SetVec3("u_LightDirection", m_Lighting.DirectionX, m_Lighting.DirectionY, m_Lighting.DirectionZ);
    m_Shader->SetFloat("u_AmbientStrength", m_Lighting.AmbientStrength);
    m_Shader->SetFloat("u_LightIntensity", m_Lighting.Intensity);

    const std::array<float, 16> gridModel = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };

    glLineWidth(1.0f);
    m_Shader->SetMat4("u_Model", gridModel);
    m_GridMesh->DrawLines();

    const auto& objects = scene.GetObjects();
    for (const SceneObject& object : objects)
    {
        m_Shader->SetMat4("u_Model", object.TransformData.GetMatrix());
        m_Shader->SetVec3("u_MaterialColor", object.MaterialData.Albedo.x, object.MaterialData.Albedo.y, object.MaterialData.Albedo.z);
        m_CubeMesh->Draw();
    }

    const int selectedIndex = scene.GetSelectedIndex();
    if (selectedIndex < 0 || selectedIndex >= static_cast<int>(objects.size()))
    {
        return;
    }

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glLineWidth(3.0f);
    glDisable(GL_CULL_FACE);
    glDepthFunc(GL_LEQUAL);

    m_Shader->SetInt("u_UseSolidColor", 1);
    m_Shader->SetVec3("u_SolidColor", 1.0f, 0.9f, 0.05f);
    m_Shader->SetMat4("u_Model", objects[selectedIndex].TransformData.GetMatrix());
    m_CubeMesh->Draw();

    m_Shader->SetInt("u_UseSolidColor", 0);
    glLineWidth(1.0f);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDepthFunc(GL_LESS);
}

}








