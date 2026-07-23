#include "Input/Input.h"

#include <GLFW/glfw3.h>

namespace Engine
{

void Input::Update(GLFWwindow* window)
{
    m_PreviousKeys = m_CurrentKeys;
    m_PreviousMouseButtons = m_CurrentMouseButtons;

    static constexpr int UsedKeys[] = {
        GLFW_KEY_ESCAPE,
        GLFW_KEY_LEFT_CONTROL,
        GLFW_KEY_RIGHT_CONTROL,
        GLFW_KEY_LEFT_SHIFT,
        GLFW_KEY_RIGHT_SHIFT,
        GLFW_KEY_TAB,
        GLFW_KEY_DELETE,
        GLFW_KEY_N,
        GLFW_KEY_W,
        GLFW_KEY_A,
        GLFW_KEY_S,
        GLFW_KEY_D,
        GLFW_KEY_SPACE,
        GLFW_KEY_C,
        GLFW_KEY_I,
        GLFW_KEY_J,
        GLFW_KEY_K,
        GLFW_KEY_L,
        GLFW_KEY_U,
        GLFW_KEY_O,
        GLFW_KEY_LEFT,
        GLFW_KEY_RIGHT,
        GLFW_KEY_UP,
        GLFW_KEY_DOWN,
        GLFW_KEY_Z,
        GLFW_KEY_X
    };

    for (int key : UsedKeys)
    {
        m_CurrentKeys[key] = glfwGetKey(window, key) == GLFW_PRESS;
    }

    for (int button = 0; button <= GLFW_MOUSE_BUTTON_LAST; button++)
    {
        m_CurrentMouseButtons[button] = glfwGetMouseButton(window, button) == GLFW_PRESS;
    }

    m_PreviousMouseX = m_MouseX;
    m_PreviousMouseY = m_MouseY;
    glfwGetCursorPos(window, &m_MouseX, &m_MouseY);

    if (!m_HasMousePosition)
    {
        m_PreviousMouseX = m_MouseX;
        m_PreviousMouseY = m_MouseY;
        m_HasMousePosition = true;
    }

    m_MouseDeltaX = m_MouseX - m_PreviousMouseX;
    m_MouseDeltaY = m_MouseY - m_PreviousMouseY;
}

bool Input::IsKeyPressed(int key) const
{
    if (key < 0 || key >= MaxKeys)
    {
        return false;
    }

    return m_CurrentKeys[key];
}

bool Input::IsKeyJustPressed(int key) const
{
    if (key < 0 || key >= MaxKeys)
    {
        return false;
    }

    return m_CurrentKeys[key] && !m_PreviousKeys[key];
}

bool Input::IsMouseButtonPressed(int button) const
{
    if (button < 0 || button > GLFW_MOUSE_BUTTON_LAST || button >= MaxMouseButtons)
    {
        return false;
    }

    return m_CurrentMouseButtons[button];
}

bool Input::IsMouseButtonJustPressed(int button) const
{
    if (button < 0 || button > GLFW_MOUSE_BUTTON_LAST || button >= MaxMouseButtons)
    {
        return false;
    }

    return m_CurrentMouseButtons[button] && !m_PreviousMouseButtons[button];
}

}
