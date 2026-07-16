#include "Platform/Window.h"

#include <GL/glew.h>
#include <iostream>

namespace Engine
{

Window::Window(int width, int height, const std::string& title)
    : m_Width(width), m_Height(height), m_Title(title)
{
}

Window::~Window()
{
    if (m_Handle)
    {
        glfwDestroyWindow(m_Handle);
        m_Handle = nullptr;
    }

    glfwTerminate();
}

bool Window::Initialize()
{
    glfwSetErrorCallback(ErrorCallback);

    if (!glfwInit())
    {
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_Handle = glfwCreateWindow(m_Width, m_Height, m_Title.c_str(), nullptr, nullptr);
    if (!m_Handle)
    {
        return false;
    }

    glfwMakeContextCurrent(m_Handle);
    glfwSwapInterval(1);
    glfwSetWindowUserPointer(m_Handle, this);
    glfwSetFramebufferSizeCallback(m_Handle, FramebufferSizeCallback);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return false;
    }

    glViewport(0, 0, m_Width, m_Height);

    std::cout << "OpenGL: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GPU: " << glGetString(GL_RENDERER) << std::endl;

    return true;
}

void Window::PollEvents() const
{
    glfwPollEvents();
}

void Window::SwapBuffers() const
{
    glfwSwapBuffers(m_Handle);
}

bool Window::ShouldClose() const
{
    return glfwWindowShouldClose(m_Handle);
}

void Window::RequestClose() const
{
    glfwSetWindowShouldClose(m_Handle, true);
}

bool Window::IsKeyPressed(int key) const
{
    return glfwGetKey(m_Handle, key) == GLFW_PRESS;
}

void Window::SetTitle(const std::string& title) const
{
    glfwSetWindowTitle(m_Handle, title.c_str());
}

void Window::SetCursorCaptured(bool captured)
{
    if (m_CursorCaptured == captured)
    {
        return;
    }

    m_CursorCaptured = captured;
    glfwSetInputMode(m_Handle, GLFW_CURSOR, captured ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
}

void Window::ErrorCallback(int error, const char* description)
{
    std::cerr << "GLFW Error " << error << ": " << description << std::endl;
}

void Window::FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    auto* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (self)
    {
        self->m_Width = width;
        self->m_Height = height;
    }

    glViewport(0, 0, width, height);
}

}
