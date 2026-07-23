#include "Editor/EditorController.h"

#include <GLFW/glfw3.h>
#include <iomanip>
#include <sstream>

namespace Engine
{

void EditorController::Update(Scene& scene, const Input& input, float deltaTime, bool allowMouseRotation)
{
    if (input.IsKeyJustPressed(GLFW_KEY_TAB))
    {
        if (input.IsKeyPressed(GLFW_KEY_LEFT_SHIFT) || input.IsKeyPressed(GLFW_KEY_RIGHT_SHIFT))
        {
            scene.SelectPrevious();
        }
        else
        {
            scene.SelectNext();
        }
    }

    SceneObject* selected = scene.GetSelectedObject();
    if (!selected)
    {
        return;
    }

    const float move = m_MoveSpeed * deltaTime;
    const float rotate = m_RotateSpeed * deltaTime;
    const float scale = m_ScaleSpeed * deltaTime;

    if (input.IsKeyPressed(GLFW_KEY_J))
    {
        selected->TransformData.Position.x -= move;
    }

    if (input.IsKeyPressed(GLFW_KEY_L))
    {
        selected->TransformData.Position.x += move;
    }

    if (input.IsKeyPressed(GLFW_KEY_I))
    {
        selected->TransformData.Position.y += move;
    }

    if (input.IsKeyPressed(GLFW_KEY_K))
    {
        selected->TransformData.Position.y -= move;
    }

    if (input.IsKeyPressed(GLFW_KEY_U))
    {
        selected->TransformData.Position.z += move;
    }

    if (input.IsKeyPressed(GLFW_KEY_O))
    {
        selected->TransformData.Position.z -= move;
    }

    if (input.IsKeyPressed(GLFW_KEY_LEFT))
    {
        selected->TransformData.Rotation.y -= rotate;
    }

    if (input.IsKeyPressed(GLFW_KEY_RIGHT))
    {
        selected->TransformData.Rotation.y += rotate;
    }

    if (input.IsKeyPressed(GLFW_KEY_UP))
    {
        selected->TransformData.Rotation.x -= rotate;
    }

    if (input.IsKeyPressed(GLFW_KEY_DOWN))
    {
        selected->TransformData.Rotation.x += rotate;
    }

    if (allowMouseRotation && input.IsMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT))
    {
        selected->TransformData.Rotation.y += static_cast<float>(input.GetMouseDeltaX()) * m_MouseRotateSpeed;
        selected->TransformData.Rotation.x += static_cast<float>(input.GetMouseDeltaY()) * m_MouseRotateSpeed;
    }

    if (input.IsKeyPressed(GLFW_KEY_Z))
    {
        selected->TransformData.Scale.x -= scale;
        selected->TransformData.Scale.y -= scale;
        selected->TransformData.Scale.z -= scale;
    }

    if (input.IsKeyPressed(GLFW_KEY_X))
    {
        selected->TransformData.Scale.x += scale;
        selected->TransformData.Scale.y += scale;
        selected->TransformData.Scale.z += scale;
    }

    if (selected->TransformData.Scale.x < 0.1f)
    {
        selected->TransformData.Scale = {0.1f, 0.1f, 0.1f};
    }
}

std::string EditorController::BuildWindowTitle(const Scene& scene) const
{
    const SceneObject* selected = scene.GetSelectedObject();
    if (!selected)
    {
        return "Renderer Engine - Scene Empty | N add cube | WASD camera";
    }

    std::ostringstream stream;
    stream << std::fixed << std::setprecision(2);
    stream << "Renderer Engine - Editing " << selected->Name;
    stream << " | Pos " << selected->TransformData.Position.x << ", " << selected->TransformData.Position.y << ", " << selected->TransformData.Position.z;
    stream << " | Camera: WASD + Space/C + RMB drag in Viewport | Object: IJKL/UO move + LMB rotate in Viewport";

    return stream.str();
}

}
