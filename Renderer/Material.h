#pragma once

#include "Scene/Transform.h"

#include <string>

namespace Engine
{

struct Material
{
    Vec3 Albedo = {1.0f, 1.0f, 1.0f};
    std::string TexturePath;
    bool UseTexture = false;
};

}
