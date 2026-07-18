#include "Renderer/Renderer.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <array>
#include <cmath>
#include <iostream>

namespace Engine
{


namespace
{

std::array<float, 16> MultiplyMatrix(const std::array<float, 16>& left, const std::array<float, 16>& right)
{
    std::array<float, 16> result = {};

    for (int column = 0; column < 4; column++)
    {
        for (int row = 0; row < 4; row++)
        {
            result[column * 4 + row] =
                left[0 * 4 + row] * right[column * 4 + 0] +
                left[1 * 4 + row] * right[column * 4 + 1] +
                left[2 * 4 + row] * right[column * 4 + 2] +
                left[3 * 4 + row] * right[column * 4 + 3];
        }
    }

    return result;
}

std::array<float, 16> CreatePlanarShadowMatrix(float planeY, float lightX, float lightY, float lightZ)
{
    if (std::abs(lightY) < 0.05f)
    {
        lightY = lightY < 0.0f ? -0.05f : 0.05f;
    }

    const float xOverY = lightX / lightY;
    const float zOverY = lightZ / lightY;

    return {
        1.0f, 0.0f, 0.0f, 0.0f,
        -xOverY, 0.0f, -zOverY, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        xOverY * planeY, planeY, zOverY * planeY, 1.0f
    };
}

}

Renderer::~Renderer()
{
    if (m_SkyVertexBuffer != 0)
    {
        glDeleteBuffers(1, &m_SkyVertexBuffer);
        m_SkyVertexBuffer = 0;
    }

    if (m_SkyVertexArray != 0)
    {
        glDeleteVertexArrays(1, &m_SkyVertexArray);
        m_SkyVertexArray = 0;
    }
}
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

    m_TerrainMesh = std::make_unique<Mesh>();
    if (!m_TerrainMesh->CreateTerrain(10, 1.0f))
    {
        std::cerr << "Failed to create terrain mesh" << std::endl;
        return false;
    }

    m_Shader = std::make_unique<Shader>();
    if (!m_Shader->LoadFromFiles("Shaders/basic.vert", "Shaders/basic.frag"))
    {
        std::cerr << "Failed to load cube shader" << std::endl;
        return false;
    }

    m_SkyShader = std::make_unique<Shader>();
    if (!m_SkyShader->LoadFromFiles("Shaders/sky.vert", "Shaders/sky.frag"))
    {
        std::cerr << "Failed to load sky shader" << std::endl;
        return false;
    }

    const float skyVertices[] = {
        -1.0f, -1.0f,
         3.0f, -1.0f,
        -1.0f,  3.0f
    };

    glGenVertexArrays(1, &m_SkyVertexArray);
    glGenBuffers(1, &m_SkyVertexBuffer);
    glBindVertexArray(m_SkyVertexArray);
    glBindBuffer(GL_ARRAY_BUFFER, m_SkyVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyVertices), skyVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);
    glBindVertexArray(0);
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
    DrawSkyBackground(m_Camera.GetYaw(), m_Camera.GetPitch(), aspectRatio);
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

void Renderer::DrawSkyBackground(float cameraYaw, float cameraPitch, float aspectRatio) const
{
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);

    m_SkyShader->Bind();
    m_SkyShader->SetFloat("u_CameraYaw", cameraYaw);
    m_SkyShader->SetFloat("u_CameraPitch", cameraPitch);
    m_SkyShader->SetFloat("u_AspectRatio", aspectRatio);
    m_SkyShader->SetVec3("u_LightDirection", m_Lighting.DirectionX, m_Lighting.DirectionY, m_Lighting.DirectionZ);
    m_SkyShader->SetFloat("u_AmbientStrength", m_Lighting.AmbientStrength);
    m_SkyShader->SetFloat("u_LightIntensity", m_Lighting.Intensity);
    glBindVertexArray(m_SkyVertexArray);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);

    glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST);
}

bool Renderer::ProjectWorldToViewport(const Vec3& worldPosition, int width, int height, float& x, float& y) const
{
    if (width <= 0 || height <= 0)
    {
        return false;
    }

    const float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
    const auto viewProjection = m_Camera.GetViewProjection(aspectRatio);

    const float clipX = viewProjection[0] * worldPosition.x + viewProjection[4] * worldPosition.y + viewProjection[8] * worldPosition.z + viewProjection[12];
    const float clipY = viewProjection[1] * worldPosition.x + viewProjection[5] * worldPosition.y + viewProjection[9] * worldPosition.z + viewProjection[13];
    const float clipW = viewProjection[3] * worldPosition.x + viewProjection[7] * worldPosition.y + viewProjection[11] * worldPosition.z + viewProjection[15];

    if (clipW <= 0.001f)
    {
        return false;
    }

    const float ndcX = clipX / clipW;
    const float ndcY = clipY / clipW;

    if (ndcX < -1.3f || ndcX > 1.3f || ndcY < -1.3f || ndcY > 1.3f)
    {
        return false;
    }

    x = (ndcX * 0.5f + 0.5f) * static_cast<float>(width);
    y = (1.0f - (ndcY * 0.5f + 0.5f)) * static_cast<float>(height);
    return true;
}

float Renderer::FindShadowPlaneY(const Scene& scene) const
{
    for (const SceneObject& object : scene.GetObjects())
    {
        if (object.Type == SceneObjectType::Terrain)
        {
            return object.TransformData.Position.y + 0.015f;
        }
    }

    return -0.705f;
}

void Renderer::DrawPlanarShadows(const Scene& scene, float shadowPlaneY) const
{
    float lightX = -m_Lighting.DirectionX;
    float lightY = -m_Lighting.DirectionY;
    float lightZ = -m_Lighting.DirectionZ;
    const float lightLength = std::sqrt(lightX * lightX + lightY * lightY + lightZ * lightZ);
    if (lightLength <= 0.0001f || lightY <= 0.01f)
    {
        return;
    }

    lightX /= lightLength;
    lightY /= lightLength;
    lightZ /= lightLength;

    const auto shadowMatrix = CreatePlanarShadowMatrix(shadowPlaneY, lightX, lightY, lightZ);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);
    glDisable(GL_CULL_FACE);

    m_Shader->SetInt("u_UseSolidColor", 1);
    m_Shader->SetVec3("u_SolidColor", 0.02f, 0.025f, 0.02f);
    m_Shader->SetFloat("u_Alpha", m_Lighting.ShadowStrength);

    for (const SceneObject& object : scene.GetObjects())
    {
        if (object.Type == SceneObjectType::Terrain || object.Type == SceneObjectType::TerrainWall)
        {
            continue;
        }

        const auto shadowModel = MultiplyMatrix(shadowMatrix, object.TransformData.GetMatrix());
        m_Shader->SetMat4("u_Model", shadowModel);
        m_CubeMesh->Draw();
    }

    m_Shader->SetFloat("u_Alpha", 1.0f);
    m_Shader->SetInt("u_UseSolidColor", 0);
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
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
    m_Shader->SetFloat("u_Alpha", 1.0f);

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
        if (object.Type != SceneObjectType::Terrain && object.Type != SceneObjectType::TerrainWall)
        {
            continue;
        }

        m_Shader->SetMat4("u_Model", object.TransformData.GetMatrix());
        m_Shader->SetVec3("u_MaterialColor", object.MaterialData.Albedo.x, object.MaterialData.Albedo.y, object.MaterialData.Albedo.z);
        m_TerrainMesh->Draw();
    }

    if (m_Lighting.ShadowsEnabled && m_Lighting.ShadowStrength > 0.01f)
    {
        DrawPlanarShadows(scene, FindShadowPlaneY(scene));
    }

    m_Shader->SetInt("u_UseSolidColor", 0);
    m_Shader->SetFloat("u_Alpha", 1.0f);
    for (const SceneObject& object : objects)
    {
        if (object.Type == SceneObjectType::Terrain || object.Type == SceneObjectType::TerrainWall)
        {
            continue;
        }

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
    m_Shader->SetFloat("u_Alpha", 1.0f);
    m_Shader->SetVec3("u_SolidColor", 1.0f, 0.9f, 0.05f);
    m_Shader->SetMat4("u_Model", objects[selectedIndex].TransformData.GetMatrix());
    if (objects[selectedIndex].Type == SceneObjectType::Terrain || objects[selectedIndex].Type == SceneObjectType::TerrainWall)
    {
        m_TerrainMesh->Draw();
    }
    else
    {
        m_CubeMesh->Draw();
    }

    m_Shader->SetInt("u_UseSolidColor", 0);
    glLineWidth(1.0f);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDepthFunc(GL_LESS);
}

}









