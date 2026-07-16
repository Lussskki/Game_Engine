#pragma once

#include <array>

struct GLFWwindow;

namespace Engine
{

class Input
{
public:
    void Update(GLFWwindow* window);

    bool IsKeyPressed(int key) const;
    bool IsKeyJustPressed(int key) const;
    bool IsMouseButtonPressed(int button) const;
    bool IsMouseButtonJustPressed(int button) const;

    double GetMouseX() const
    {
        return m_MouseX;
    }

    double GetMouseY() const
    {
        return m_MouseY;
    }

    double GetMouseDeltaX() const
    {
        return m_MouseDeltaX;
    }

    double GetMouseDeltaY() const
    {
        return m_MouseDeltaY;
    }

private:
    static constexpr int MaxKeys = 512;
    static constexpr int MaxMouseButtons = 16;

    std::array<bool, MaxKeys> m_CurrentKeys = {};
    std::array<bool, MaxKeys> m_PreviousKeys = {};
    std::array<bool, MaxMouseButtons> m_CurrentMouseButtons = {};
    std::array<bool, MaxMouseButtons> m_PreviousMouseButtons = {};

    double m_MouseX = 0.0;
    double m_MouseY = 0.0;
    double m_PreviousMouseX = 0.0;
    double m_PreviousMouseY = 0.0;
    double m_MouseDeltaX = 0.0;
    double m_MouseDeltaY = 0.0;
    bool m_HasMousePosition = false;
};

}
