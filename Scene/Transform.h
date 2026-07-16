#pragma once

#include <array>

namespace Engine
{

struct Vec3
{
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
};

class Transform
{
public:
    Vec3 Position;
    Vec3 Rotation;
    Vec3 Scale = {1.0f, 1.0f, 1.0f};

    std::array<float, 16> GetMatrix() const;

private:
    static std::array<float, 16> Multiply(const std::array<float, 16>& left, const std::array<float, 16>& right);
};

}
