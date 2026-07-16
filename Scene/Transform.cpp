#include "Scene/Transform.h"

#include <cmath>

namespace Engine
{

std::array<float, 16> Transform::GetMatrix() const
{
    const float cx = std::cos(Rotation.x);
    const float sx = std::sin(Rotation.x);
    const float cy = std::cos(Rotation.y);
    const float sy = std::sin(Rotation.y);
    const float cz = std::cos(Rotation.z);
    const float sz = std::sin(Rotation.z);

    const std::array<float, 16> scale = {
        Scale.x, 0.0f, 0.0f, 0.0f,
        0.0f, Scale.y, 0.0f, 0.0f,
        0.0f, 0.0f, Scale.z, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };

    const std::array<float, 16> rotationX = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, cx, sx, 0.0f,
        0.0f, -sx, cx, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };

    const std::array<float, 16> rotationY = {
        cy, 0.0f, -sy, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        sy, 0.0f, cy, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };

    const std::array<float, 16> rotationZ = {
        cz, sz, 0.0f, 0.0f,
        -sz, cz, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };

    const std::array<float, 16> translation = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        Position.x, Position.y, Position.z, 1.0f
    };

    return Multiply(translation, Multiply(rotationZ, Multiply(rotationY, Multiply(rotationX, scale))));
}

std::array<float, 16> Transform::Multiply(const std::array<float, 16>& left, const std::array<float, 16>& right)
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
