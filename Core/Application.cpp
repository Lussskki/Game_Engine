#include "Core/Application.h"
#include "ThirdParty/stb_image.h"

#include <GLFW/glfw3.h>
#include <algorithm>
#include <cmath>
#include <iostream>

namespace Engine
{

Application::Application() = default;
Application::~Application() = default;

bool Application::Initialize()
{
    m_Window = std::make_unique<Window>(1280, 720, "Merso");
    if (!m_Window->Initialize())
    {
        std::cerr << "Failed to initialize window" << std::endl;
        return false;
    }

        // Set window icon
        GLFWimage icon;
        icon.pixels = stbi_load("Assets/Logo/Merso.png",
                                &icon.width,
                                &icon.height,
                                nullptr,
                                4);

        if (!icon.pixels)
        {
            std::cout << "Failed to load icon: " << stbi_failure_reason() << std::endl;
        }
        else
        {
            glfwSetWindowIcon(m_Window->GetNativeWindow(), 1, &icon);
            stbi_image_free(icon.pixels);
        }

    m_Renderer = std::make_unique<Renderer>();
    if (!m_Renderer->Initialize())
    {
        std::cerr << "Failed to initialize renderer" << std::endl;
        return false;
    }

    if (!m_Gui.Initialize(m_Window->GetNativeWindow()))
    {
        std::cerr << "Failed to initialize editor GUI" << std::endl;
        return false;
    }

    m_Running = true;
    return true;
}

void Application::Run()
{
    float lastTime = static_cast<float>(glfwGetTime());

    while (m_Running && !m_Window->ShouldClose())
    {
        const float currentTime = static_cast<float>(glfwGetTime());
        const float deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        m_Window->PollEvents();
        m_Input.Update(m_Window->GetNativeWindow());
        m_Gui.BeginFrame();

        const bool viewportHovered = m_Gui.IsViewportHovered();
        const bool cameraToolActive = m_Gui.IsCameraToolSelected();
        const bool cameraLookActive = viewportHovered && m_Input.IsMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT);
        const bool moveToolActive = m_Gui.IsMoveToolSelected();
        const bool rotateToolActive = m_Gui.IsRotateToolSelected();
        const bool cameraPanActive = viewportHovered &&
            m_Input.IsMouseButtonPressed(GLFW_MOUSE_BUTTON_MIDDLE);
        const bool textInputActive = m_Gui.WantsTextInput() || m_Gui.IsEditingText();
        const bool keyboardCaptured = m_Gui.WantsKeyboardCapture() || textInputActive;

        m_Window->SetCursorCaptured(cameraLookActive);

        if (viewportHovered)
        {
            if (!m_Gui.IsPlaying() || cameraLookActive)
            {
                m_Renderer->UpdateCamera(m_Input, deltaTime, cameraLookActive);
            }

            if (cameraPanActive)
            {
                m_Renderer->PanCamera(
                    static_cast<float>(-m_Input.GetMouseDeltaX()) * 0.01f,
                    static_cast<float>(m_Input.GetMouseDeltaY()) * 0.01f
                );
            }

            const float wheel = m_Gui.GetMouseWheel();
            if (wheel != 0.0f && !moveToolActive && !rotateToolActive)
            {
                m_Renderer->ZoomCamera(wheel * 0.8f);
            }
        }

        if (m_Gui.IsPlaying())
        {
            UpdatePlayMode(deltaTime, textInputActive, cameraLookActive);
        }
        else if (!keyboardCaptured)
        {
            m_Editor.Update(m_Scene, m_Input, deltaTime, viewportHovered);
        }

        m_Renderer->RenderSceneToViewport(m_Scene, m_Gui.GetViewportWidth(), m_Gui.GetViewportHeight());
        m_Renderer->BeginScreenFrame(m_Window->GetWidth(), m_Window->GetHeight(), 0.035f, 0.038f, 0.045f, 1.0f);

        m_Gui.Draw(m_Scene, *m_Renderer, deltaTime, m_Renderer->GetViewportTextureId());
        m_Gui.EndFrame();

        m_Window->SwapBuffers();
    }
}

int Application::FindCharacterIndex() const
{
    const auto& objects = m_Scene.GetObjects();
    for (int index = 0; index < static_cast<int>(objects.size()); index++)
    {
        if (objects[index].Type == SceneObjectType::Character)
        {
            return index;
        }
    }

    return -1;
}

float Application::FindGroundY(float x, float z, float fallbackY) const
{
    float groundY = fallbackY;
    bool foundGround = false;

    for (const SceneObject& object : m_Scene.GetObjects())
    {
        if (object.CollisionShape != CollisionShapeType::Terrain || object.Type != SceneObjectType::Terrain)
        {
            continue;
        }

        if (std::abs(object.TransformData.Rotation.x) > 0.1f || std::abs(object.TransformData.Rotation.z) > 0.1f)
        {
            continue;
        }

        const float halfWidth = 10.0f * object.TransformData.Scale.x;
        const float halfDepth = 10.0f * object.TransformData.Scale.z;
        const bool insideX = x >= object.TransformData.Position.x - halfWidth && x <= object.TransformData.Position.x + halfWidth;
        const bool insideZ = z >= object.TransformData.Position.z - halfDepth && z <= object.TransformData.Position.z + halfDepth;
        const bool belowCharacterFeet = object.TransformData.Position.y <= fallbackY + 0.18f;
        if (insideX && insideZ && belowCharacterFeet && (!foundGround || object.TransformData.Position.y > groundY))
        {
            groundY = object.TransformData.Position.y;
            foundGround = true;
        }
    }

    return groundY;
}


float Application::FindCharacterSupportY(const SceneObject& character, float x, float z, float fallbackY) const
{
    const float characterRadius = 0.75f * std::max(character.TransformData.Scale.x, character.TransformData.Scale.z);
    const float characterHalfHeight = 0.75f * character.TransformData.Scale.y;
    float supportY = FindGroundY(x, z, fallbackY - characterHalfHeight) + characterHalfHeight;

    for (const SceneObject& object : m_Scene.GetObjects())
    {
        if (&object == &character || object.CollisionShape != CollisionShapeType::Box || object.Type == SceneObjectType::Character)
        {
            continue;
        }

        const float halfX = 0.7f * object.TransformData.Scale.x + characterRadius;
        const float halfZ = 0.7f * object.TransformData.Scale.z + characterRadius;
        const bool insideX = x >= object.TransformData.Position.x - halfX && x <= object.TransformData.Position.x + halfX;
        const bool insideZ = z >= object.TransformData.Position.z - halfZ && z <= object.TransformData.Position.z + halfZ;
        if (!insideX || !insideZ)
        {
            continue;
        }

        const float topY = object.TransformData.Position.y + 0.7f * object.TransformData.Scale.y + characterHalfHeight;
        if (topY > supportY && character.TransformData.Position.y >= topY - 0.25f)
        {
            supportY = topY;
        }
    }

    return supportY;
}
bool Application::IsBlockedByCollision(const SceneObject& character, const Vec3& candidatePosition) const
{
    const float characterRadius = 0.75f * std::max(character.TransformData.Scale.x, character.TransformData.Scale.z);
    const float characterHalfHeight = 0.75f * character.TransformData.Scale.y;

    for (const SceneObject& object : m_Scene.GetObjects())
    {
        if (&object == &character || object.CollisionShape != CollisionShapeType::Box || object.Type == SceneObjectType::Character)
        {
            continue;
        }

        const float halfX = 0.7f * object.TransformData.Scale.x + characterRadius;
        const float halfY = 0.7f * object.TransformData.Scale.y + characterHalfHeight;
        const float halfZ = 0.7f * object.TransformData.Scale.z + characterRadius;

        const bool overlapX = std::abs(candidatePosition.x - object.TransformData.Position.x) < halfX;
        const bool overlapY = std::abs(candidatePosition.y - object.TransformData.Position.y) < halfY;
        const bool overlapZ = std::abs(candidatePosition.z - object.TransformData.Position.z) < halfZ;
        if (overlapX && overlapY && overlapZ)
        {
            return true;
        }
    }

    return false;
}


void Application::ResolveCharacterCollisions(SceneObject& character)
{
    for (int pass = 0; pass < 4; pass++)
    {
        bool resolvedAny = false;
        const float characterRadius = 0.75f * std::max(character.TransformData.Scale.x, character.TransformData.Scale.z);
        const float characterHalfHeight = 0.75f * character.TransformData.Scale.y;

        for (const SceneObject& object : m_Scene.GetObjects())
        {
            if (&object == &character || object.Type == SceneObjectType::Character)
            {
                continue;
            }

            if (object.CollisionShape == CollisionShapeType::Terrain && object.Type == SceneObjectType::Terrain)
            {
                constexpr float HalfPi = 1.57079632679f;
                const bool wallAcrossX = std::abs(object.TransformData.Rotation.x - HalfPi) < 0.1f;
                const bool wallAcrossZ = std::abs(object.TransformData.Rotation.z - HalfPi) < 0.1f;

                if (!wallAcrossX && !wallAcrossZ)
                {
                    const float halfWidth = 10.0f * object.TransformData.Scale.x + characterRadius;
                    const float halfDepth = 10.0f * object.TransformData.Scale.z + characterRadius;
                    const bool insideX = character.TransformData.Position.x >= object.TransformData.Position.x - halfWidth && character.TransformData.Position.x <= object.TransformData.Position.x + halfWidth;
                    const bool insideZ = character.TransformData.Position.z >= object.TransformData.Position.z - halfDepth && character.TransformData.Position.z <= object.TransformData.Position.z + halfDepth;
                    const float deltaY = character.TransformData.Position.y - object.TransformData.Position.y;
                    const float overlapY = characterHalfHeight - std::abs(deltaY);

                    if (insideX && insideZ && overlapY > 0.0f)
                    {
                        const float direction = deltaY < 0.0f ? -1.0f : 1.0f;
                        character.TransformData.Position.y += direction * (overlapY + 0.01f);

                        if (direction > 0.0f)
                        {
                            m_CharacterVerticalVelocity = 0.0f;
                            m_CharacterGrounded = true;
                        }
                        else if (m_CharacterVerticalVelocity > 0.0f)
                        {
                            m_CharacterVerticalVelocity = 0.0f;
                        }

                        resolvedAny = true;
                    }

                    continue;
                }

                if (wallAcrossX)
                {
                    const float halfWidth = 10.0f * object.TransformData.Scale.x + characterRadius;
                    const float halfHeight = 10.0f * object.TransformData.Scale.z + characterHalfHeight;
                    const bool insideX = character.TransformData.Position.x >= object.TransformData.Position.x - halfWidth && character.TransformData.Position.x <= object.TransformData.Position.x + halfWidth;
                    const bool insideY = character.TransformData.Position.y >= object.TransformData.Position.y - halfHeight && character.TransformData.Position.y <= object.TransformData.Position.y + halfHeight;
                    const float deltaZ = character.TransformData.Position.z - object.TransformData.Position.z;
                    const float overlapZ = characterRadius - std::abs(deltaZ);

                    if (insideX && insideY && overlapZ > 0.0f)
                    {
                        const float direction = deltaZ < 0.0f ? -1.0f : 1.0f;
                        character.TransformData.Position.z += direction * (overlapZ + 0.01f);
                        resolvedAny = true;
                    }
                }
                else
                {
                    const float halfWidth = 10.0f * object.TransformData.Scale.z + characterRadius;
                    const float halfHeight = 10.0f * object.TransformData.Scale.x + characterHalfHeight;
                    const bool insideZ = character.TransformData.Position.z >= object.TransformData.Position.z - halfWidth && character.TransformData.Position.z <= object.TransformData.Position.z + halfWidth;
                    const bool insideY = character.TransformData.Position.y >= object.TransformData.Position.y - halfHeight && character.TransformData.Position.y <= object.TransformData.Position.y + halfHeight;
                    const float deltaX = character.TransformData.Position.x - object.TransformData.Position.x;
                    const float overlapX = characterRadius - std::abs(deltaX);

                    if (insideZ && insideY && overlapX > 0.0f)
                    {
                        const float direction = deltaX < 0.0f ? -1.0f : 1.0f;
                        character.TransformData.Position.x += direction * (overlapX + 0.01f);
                        resolvedAny = true;
                    }
                }

                continue;
            }

            if (object.CollisionShape != CollisionShapeType::Box)
            {
                continue;
            }

            const float halfX = 0.7f * object.TransformData.Scale.x + characterRadius;
            const float halfY = 0.7f * object.TransformData.Scale.y + characterHalfHeight;
            const float halfZ = 0.7f * object.TransformData.Scale.z + characterRadius;

            const float deltaX = character.TransformData.Position.x - object.TransformData.Position.x;
            const float deltaY = character.TransformData.Position.y - object.TransformData.Position.y;
            const float deltaZ = character.TransformData.Position.z - object.TransformData.Position.z;

            const float overlapX = halfX - std::abs(deltaX);
            const float overlapY = halfY - std::abs(deltaY);
            const float overlapZ = halfZ - std::abs(deltaZ);

            if (overlapX <= 0.0f || overlapY <= 0.0f || overlapZ <= 0.0f)
            {
                continue;
            }

            if (overlapY <= overlapX && overlapY <= overlapZ)
            {
                const float direction = deltaY < 0.0f ? -1.0f : 1.0f;
                character.TransformData.Position.y += direction * (overlapY + 0.01f);

                if (direction > 0.0f)
                {
                    m_CharacterVerticalVelocity = 0.0f;
                    m_CharacterGrounded = true;
                }
                else if (m_CharacterVerticalVelocity > 0.0f)
                {
                    m_CharacterVerticalVelocity = 0.0f;
                }
            }
            else if (overlapX < overlapZ)
            {
                const float direction = deltaX < 0.0f ? -1.0f : 1.0f;
                character.TransformData.Position.x += direction * (overlapX + 0.01f);
            }
            else
            {
                const float direction = deltaZ < 0.0f ? -1.0f : 1.0f;
                character.TransformData.Position.z += direction * (overlapZ + 0.01f);
            }

            resolvedAny = true;
        }

        if (!resolvedAny)
        {
            return;
        }
    }
}
void Application::UpdatePlayMode(float deltaTime, bool textInputActive, bool cameraLookActive)
{
    if (textInputActive || cameraLookActive)
    {
        return;
    }

    const int characterIndex = FindCharacterIndex();
    if (characterIndex < 0)
    {
        return;
    }

    SceneObject& character = m_Scene.GetObjects()[characterIndex];
    ResolveCharacterCollisions(character);

    Vec3 movement;
    if (m_Input.IsKeyPressed(GLFW_KEY_W))
    {
        movement.z -= 1.0f;
    }

    if (m_Input.IsKeyPressed(GLFW_KEY_S))
    {
        movement.z += 1.0f;
    }

    if (m_Input.IsKeyPressed(GLFW_KEY_A))
    {
        movement.x -= 1.0f;
    }

    if (m_Input.IsKeyPressed(GLFW_KEY_D))
    {
        movement.x += 1.0f;
    }

    const float groundY = FindCharacterSupportY(character, character.TransformData.Position.x, character.TransformData.Position.z, character.TransformData.Position.y);
    m_CharacterGrounded = character.TransformData.Position.y <= groundY + 0.03f;
    if (m_CharacterGrounded)
    {
        character.TransformData.Position.y = groundY;
        if (m_CharacterVerticalVelocity < 0.0f)
        {
            m_CharacterVerticalVelocity = 0.0f;
        }
    }

    if (m_CharacterGrounded && m_Input.IsKeyJustPressed(GLFW_KEY_SPACE))
    {
        m_CharacterVerticalVelocity = 4.8f;
        m_CharacterGrounded = false;
    }

    const float length = std::sqrt(movement.x * movement.x + movement.z * movement.z);
    if (length > 0.0f)
    {
        movement.x /= length;
        movement.z /= length;

        const float speed = 3.0f * deltaTime;
        Vec3 candidate = character.TransformData.Position;
        candidate.x += movement.x * speed;
        if (!IsBlockedByCollision(character, candidate))
        {
            character.TransformData.Position = candidate;
            ResolveCharacterCollisions(character);
        }

        candidate = character.TransformData.Position;
        candidate.z += movement.z * speed;
        if (!IsBlockedByCollision(character, candidate))
        {
            character.TransformData.Position = candidate;
            ResolveCharacterCollisions(character);
        }
    }

    m_CharacterVerticalVelocity -= 9.8f * deltaTime;
    character.TransformData.Position.y += m_CharacterVerticalVelocity * deltaTime;

    const float nextGroundY = FindCharacterSupportY(character, character.TransformData.Position.x, character.TransformData.Position.z, character.TransformData.Position.y);
    if (character.TransformData.Position.y <= nextGroundY)
    {
        character.TransformData.Position.y = nextGroundY;
        m_CharacterVerticalVelocity = 0.0f;
        m_CharacterGrounded = true;
    }

    ResolveCharacterCollisions(character);
}
void Application::Shutdown()
{
    m_Gui.Shutdown();
    m_Renderer.reset();
    m_Window.reset();
    m_Running = false;
}

}
















