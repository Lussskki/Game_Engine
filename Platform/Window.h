#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <string>

namespace Engine
{

class Window
{
public:
    Window(int width, int height, const std::string& title);
    ~Window();

    bool Initialize();
    void PollEvents() const;
    void SwapBuffers() const;
    bool ShouldClose() const;
    void RequestClose() const;
    bool IsKeyPressed(int key) const;
    void SetTitle(const std::string& title) const;
    void SetCursorCaptured(bool captured);

    int GetWidth() const
    {
        return m_Width;
    }

    int GetHeight() const
    {
        return m_Height;
    }

    GLFWwindow* GetNativeWindow() const
    {
        return m_Handle;
    }

private:
    static void ErrorCallback(int error, const char* description);
    static void FramebufferSizeCallback(GLFWwindow* window, int width, int height);

    int m_Width = 0;
    int m_Height = 0;
    std::string m_Title;
    GLFWwindow* m_Handle = nullptr;
    bool m_CursorCaptured = false;
};

}
