#include "Renderer/Camera.h"

#include <algorithm>
#include <cmath>

namespace Engine
{

void Camera::MoveForward(float amount)
{
    const Vector3 forward = GetForward();
    m_Position.x += forward.x * amount;
    m_Position.y += forward.y * amount;
    m_Position.z += forward.z * amount;
}

void Camera::MoveRight(float amount)
{
    const Vector3 right = GetRight();
    m_Position.x += right.x * amount;
    m_Position.y += right.y * amount;
    m_Position.z += right.z * amount;
}

void Camera::MoveUp(float amount)
{
    m_Position.y += amount;
}

void Camera::Rotate(float yawDelta, float pitchDelta)
{
    m_Yaw += yawDelta;
    m_Pitch += pitchDelta;
    m_Pitch = std::clamp(m_Pitch, -1.45f, 1.45f);
}

std::array<float, 16> Camera::GetViewProjection(float aspectRatio) const
{
    const float fieldOfView = 45.0f * 3.1415926535f / 180.0f;
    const auto projection = Perspective(fieldOfView, aspectRatio, 0.1f, 100.0f);
    const Vector3 forward = GetForward();
    const Vector3 target = {
        m_Position.x + forward.x,
        m_Position.y + forward.y,
        m_Position.z + forward.z
    };
    const auto view = LookAt(m_Position, target, {0.0f, 1.0f, 0.0f});

    return Multiply(projection, view);
}

Camera::Vector3 Camera::GetForward() const
{
    return Normalize({
        std::cos(m_Yaw) * std::cos(m_Pitch),
        std::sin(m_Pitch),
        std::sin(m_Yaw) * std::cos(m_Pitch)
    });
}

Camera::Vector3 Camera::GetRight() const
{
    return Normalize(Cross(GetForward(), {0.0f, 1.0f, 0.0f}));
}

Camera::Vector3 Camera::Normalize(Vector3 value)
{
    const float length = std::sqrt(value.x * value.x + value.y * value.y + value.z * value.z);
    if (length <= 0.00001f)
    {
        return {};
    }

    return {value.x / length, value.y / length, value.z / length};
}

Camera::Vector3 Camera::Cross(Vector3 left, Vector3 right)
{
    return {
        left.y * right.z - left.z * right.y,
        left.z * right.x - left.x * right.z,
        left.x * right.y - left.y * right.x
    };
}

float Camera::Dot(Vector3 left, Vector3 right)
{
    return left.x * right.x + left.y * right.y + left.z * right.z;
}

std::array<float, 16> Camera::Perspective(float fieldOfViewRadians, float aspectRatio, float nearPlane, float farPlane)
{
    const float tanHalfFov = std::tan(fieldOfViewRadians * 0.5f);

    return {
        1.0f / (aspectRatio * tanHalfFov), 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f / tanHalfFov, 0.0f, 0.0f,
        0.0f, 0.0f, -(farPlane + nearPlane) / (farPlane - nearPlane), -1.0f,
        0.0f, 0.0f, -(2.0f * farPlane * nearPlane) / (farPlane - nearPlane), 0.0f
    };
}

std::array<float, 16> Camera::LookAt(Vector3 position, Vector3 target, Vector3 up)
{
    const Vector3 forward = Normalize({
        target.x - position.x,
        target.y - position.y,
        target.z - position.z
    });
    const Vector3 side = Normalize(Cross(forward, up));
    const Vector3 cameraUp = Cross(side, forward);

    return {
        side.x, cameraUp.x, -forward.x, 0.0f,
        side.y, cameraUp.y, -forward.y, 0.0f,
        side.z, cameraUp.z, -forward.z, 0.0f,
        -Dot(side, position), -Dot(cameraUp, position), Dot(forward, position), 1.0f
    };
}

std::array<float, 16> Camera::Multiply(const std::array<float, 16>& left, const std::array<float, 16>& right)
{
    std::array<float, 16> result = {};

    for (int column = 0; column < 4; column++)
    {
        for (int row = 0; row < 4; row++)
        {
            result[column * 4 + row] =
                left[0 * 4 + row] * right[column * 4 + 0] +
                left[1 * 4 + row] * right[column * 4 + 1] +
                left[2 * 4 + row] * right[column * 4 + 2] +
                left[3 * 4 + row] * right[column * 4 + 3];
        }
    }

    return result;
}

}
