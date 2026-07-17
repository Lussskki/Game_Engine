#pragma once

#include "Renderer/Mesh.h"

#include <string>

namespace Engine
{

class ModelLoader
{
public:
    static bool LoadModel(const std::string& path, Mesh& mesh);
};

}
