#pragma once

#include "Renderer/IndexBuffer.h"
#include "Renderer/VertexArray.h"
#include "Renderer/VertexBuffer.h"

#include <memory>

namespace Engine
{

class Mesh
{
public:
    bool CreateCube();
    bool CreateGrid(int halfSize, float spacing, float yPosition);
    bool CreateTerrain(int halfSize, float spacing);
    void Draw() const;
    void DrawLines() const;

private:
    std::unique_ptr<VertexArray> m_VertexArray;
    std::unique_ptr<VertexBuffer> m_VertexBuffer;
    std::unique_ptr<IndexBuffer> m_IndexBuffer;
};

}

