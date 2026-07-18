#pragma once

#include "Scene/Transform.h"

#include <array>

namespace Engine
{

class Camera
{
public:
    void MoveForward(float amount);
    void MoveRight(float amount);
    void MoveUp(float amount);
    void Rotate(float yawDelta, float pitchDelta);
    
    std::array<float, 16> GetViewProjection(float aspectRatio) const;
    Vec3 GetPosition() const;
    Vec3 GetForwardDirection() const;
    Vec3 GetRightDirection() const;
    Vec3 GetUpDirection() const;
    float GetPitch() const;
    float GetYaw() const;

private:
    struct Vector3
    {
        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;
    };

    Vector3 GetForward() const;
    Vector3 GetRight() const;

    static Vector3 Normalize(Vector3 value);
    static Vector3 Cross(Vector3 left, Vector3 right);
    static float Dot(Vector3 left, Vector3 right);
    static std::array<float, 16> Perspective(float fieldOfViewRadians, float aspectRatio, float nearPlane, float farPlane);
    static std::array<float, 16> LookAt(Vector3 position, Vector3 target, Vector3 up);
    static std::array<float, 16> Multiply(const std::array<float, 16>& left, const std::array<float, 16>& right);

    Vector3 m_Position = {0.0f, 0.0f, 4.0f};
    float m_Yaw = -1.57079632679f;
    float m_Pitch = 0.0f;
};

}

