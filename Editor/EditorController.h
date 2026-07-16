#pragma once

#include "Input/Input.h"
#include "Scene/Scene.h"

#include <string>

namespace Engine
{

class EditorController
{
public:
    void Update(Scene& scene, const Input& input, float deltaTime, bool allowMouseRotation);
    std::string BuildWindowTitle(const Scene& scene) const;

private:
    float m_MoveSpeed = 2.0f;
    float m_RotateSpeed = 1.8f;
    float m_MouseRotateSpeed = 0.01f;
    float m_ScaleSpeed = 1.0f;
};

}
