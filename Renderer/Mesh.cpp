#include "Renderer/Mesh.h"

#include <GL/glew.h>

#include <cmath>
#include <cstddef>
#include <vector>

namespace Engine
{

struct MeshVertex
{
    float x, y, z;
    float r, g, b;
};

bool Mesh::CreateCube()
{
    const MeshVertex vertices[] = {
        {-0.7f, -0.7f,  0.7f, 1.0f, 0.2f, 0.2f},
        { 0.7f, -0.7f,  0.7f, 0.2f, 1.0f, 0.3f},
        { 0.7f,  0.7f,  0.7f, 0.2f, 0.5f, 1.0f},
        {-0.7f,  0.7f,  0.7f, 1.0f, 0.9f, 0.2f},
        {-0.7f, -0.7f, -0.7f, 1.0f, 0.2f, 1.0f},
        { 0.7f, -0.7f, -0.7f, 0.2f, 1.0f, 1.0f},
        { 0.7f,  0.7f, -0.7f, 0.9f, 0.9f, 1.0f},
        {-0.7f,  0.7f, -0.7f, 1.0f, 0.5f, 0.2f}
    };

    const unsigned int indices[] = {
        0, 1, 2, 2, 3, 0,
        1, 5, 6, 6, 2, 1,
        5, 4, 7, 7, 6, 5,
        4, 0, 3, 3, 7, 4,
        3, 2, 6, 6, 7, 3,
        4, 5, 1, 1, 0, 4
    };

    m_VertexArray = std::make_unique<VertexArray>();
    m_VertexBuffer = std::make_unique<VertexBuffer>();
    m_IndexBuffer = std::make_unique<IndexBuffer>();

    m_VertexArray->Create();
    m_VertexArray->Bind();

    m_VertexBuffer->Create(vertices, sizeof(vertices));
    m_IndexBuffer->Create(indices, 36);

    m_VertexArray->AddFloatAttribute(0, 3, sizeof(MeshVertex), reinterpret_cast<const void*>(offsetof(MeshVertex, x)));
    m_VertexArray->AddFloatAttribute(1, 3, sizeof(MeshVertex), reinterpret_cast<const void*>(offsetof(MeshVertex, r)));

    m_VertexArray->Unbind();
    return true;
}

bool Mesh::CreateCircle()
{
    std::vector<MeshVertex> vertices;
    std::vector<unsigned int> indices;

    const int latitudeCount = 24;
    const int longitudeCount = 32;
    const float radius = 0.75f;

    for (int latitude = 0; latitude <= latitudeCount; latitude++)
    {
        const float v = static_cast<float>(latitude) / static_cast<float>(latitudeCount);
        const float pitch = (v - 0.5f) * 3.14159265359f;
        const float ringRadius = std::cos(pitch) * radius;
        const float y = std::sin(pitch) * radius;

        for (int longitude = 0; longitude <= longitudeCount; longitude++)
        {
            const float u = static_cast<float>(longitude) / static_cast<float>(longitudeCount);
            const float yaw = u * 6.28318530718f;
            const float x = std::cos(yaw) * ringRadius;
            const float z = std::sin(yaw) * ringRadius;
            const float colorMix = 0.25f + v * 0.55f;
            vertices.push_back({x, y, z, 0.20f + colorMix, 0.45f + colorMix * 0.45f, 1.0f});
        }
    }

    const int rowSize = longitudeCount + 1;
    for (int latitude = 0; latitude < latitudeCount; latitude++)
    {
        for (int longitude = 0; longitude < longitudeCount; longitude++)
        {
            const unsigned int topLeft = static_cast<unsigned int>(latitude * rowSize + longitude);
            const unsigned int topRight = topLeft + 1;
            const unsigned int bottomLeft = static_cast<unsigned int>((latitude + 1) * rowSize + longitude);
            const unsigned int bottomRight = bottomLeft + 1;

            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);
            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
        }
    }

    m_VertexArray = std::make_unique<VertexArray>();
    m_VertexBuffer = std::make_unique<VertexBuffer>();
    m_IndexBuffer = std::make_unique<IndexBuffer>();

    m_VertexArray->Create();
    m_VertexArray->Bind();

    m_VertexBuffer->Create(vertices.data(), static_cast<unsigned int>(vertices.size() * sizeof(MeshVertex)));
    m_IndexBuffer->Create(indices.data(), static_cast<unsigned int>(indices.size()));

    m_VertexArray->AddFloatAttribute(0, 3, sizeof(MeshVertex), reinterpret_cast<const void*>(offsetof(MeshVertex, x)));
    m_VertexArray->AddFloatAttribute(1, 3, sizeof(MeshVertex), reinterpret_cast<const void*>(offsetof(MeshVertex, r)));

    m_VertexArray->Unbind();
    return true;
}
bool Mesh::CreateGrid(int halfSize, float spacing, float yPosition)
{
    std::vector<MeshVertex> vertices;
    std::vector<unsigned int> indices;

    const float extent = static_cast<float>(halfSize) * spacing;
    unsigned int index = 0;

    for (int line = -halfSize; line <= halfSize; line++)
    {
        const float position = static_cast<float>(line) * spacing;
        const bool axisLine = line == 0;
        const float brightness = axisLine ? 0.55f : 0.32f;

        vertices.push_back({-extent, yPosition, position, brightness, brightness, brightness});
        vertices.push_back({ extent, yPosition, position, brightness, brightness, brightness});
        indices.push_back(index++);
        indices.push_back(index++);

        vertices.push_back({position, yPosition, -extent, brightness, brightness, brightness});
        vertices.push_back({position, yPosition,  extent, brightness, brightness, brightness});
        indices.push_back(index++);
        indices.push_back(index++);
    }

    m_VertexArray = std::make_unique<VertexArray>();
    m_VertexBuffer = std::make_unique<VertexBuffer>();
    m_IndexBuffer = std::make_unique<IndexBuffer>();

    m_VertexArray->Create();
    m_VertexArray->Bind();

    m_VertexBuffer->Create(vertices.data(), static_cast<unsigned int>(vertices.size() * sizeof(MeshVertex)));
    m_IndexBuffer->Create(indices.data(), static_cast<unsigned int>(indices.size()));

    m_VertexArray->AddFloatAttribute(0, 3, sizeof(MeshVertex), reinterpret_cast<const void*>(offsetof(MeshVertex, x)));
    m_VertexArray->AddFloatAttribute(1, 3, sizeof(MeshVertex), reinterpret_cast<const void*>(offsetof(MeshVertex, r)));

    m_VertexArray->Unbind();
    return true;
}


bool Mesh::CreateTerrain(int halfSize, float spacing)
{
    std::vector<MeshVertex> vertices;
    std::vector<unsigned int> indices;

    const int vertexCount = halfSize * 2 + 1;
    const float extent = static_cast<float>(halfSize) * spacing;

    for (int z = 0; z < vertexCount; z++)
    {
        for (int x = 0; x < vertexCount; x++)
        {
            const float worldX = -extent + static_cast<float>(x) * spacing;
            const float worldZ = -extent + static_cast<float>(z) * spacing;
            const float checker = ((x + z) % 2 == 0) ? 0.08f : 0.0f;
            vertices.push_back({worldX, 0.0f, worldZ, 0.28f + checker, 0.48f + checker, 0.24f + checker});
        }
    }

    for (int z = 0; z < vertexCount - 1; z++)
    {
        for (int x = 0; x < vertexCount - 1; x++)
        {
            const unsigned int topLeft = static_cast<unsigned int>(z * vertexCount + x);
            const unsigned int topRight = topLeft + 1;
            const unsigned int bottomLeft = static_cast<unsigned int>((z + 1) * vertexCount + x);
            const unsigned int bottomRight = bottomLeft + 1;

            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);
            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
        }
    }

    m_VertexArray = std::make_unique<VertexArray>();
    m_VertexBuffer = std::make_unique<VertexBuffer>();
    m_IndexBuffer = std::make_unique<IndexBuffer>();

    m_VertexArray->Create();
    m_VertexArray->Bind();

    m_VertexBuffer->Create(vertices.data(), static_cast<unsigned int>(vertices.size() * sizeof(MeshVertex)));
    m_IndexBuffer->Create(indices.data(), static_cast<unsigned int>(indices.size()));

    m_VertexArray->AddFloatAttribute(0, 3, sizeof(MeshVertex), reinterpret_cast<const void*>(offsetof(MeshVertex, x)));
    m_VertexArray->AddFloatAttribute(1, 3, sizeof(MeshVertex), reinterpret_cast<const void*>(offsetof(MeshVertex, r)));

    m_VertexArray->Unbind();
    return true;
}

void Mesh::Draw() const
{
    m_VertexArray->Bind();
    m_IndexBuffer->Bind();
    glDrawElements(GL_TRIANGLES, m_IndexBuffer->GetCount(), GL_UNSIGNED_INT, nullptr);
}

void Mesh::DrawLines() const
{
    m_VertexArray->Bind();
    m_IndexBuffer->Bind();
    glDrawElements(GL_LINES, m_IndexBuffer->GetCount(), GL_UNSIGNED_INT, nullptr);
}

}



