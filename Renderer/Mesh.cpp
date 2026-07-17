#include "Renderer/Mesh.h"

#include <GL/glew.h>

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

