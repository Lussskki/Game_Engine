#include "Renderer/ModelLoader.h"

#include "Core/ConsoleLog.h"

namespace Engine
{

bool ModelLoader::LoadModel(const std::string& path, Mesh& mesh)
{
    (void)mesh;
    ConsoleLog::Warning("Assimp is not installed/enabled, so model loading is disabled: " + path);
    return false;
}

}
