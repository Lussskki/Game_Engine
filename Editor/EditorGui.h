#pragma once

#include "Scene/Scene.h"

struct GLFWwindow;

namespace Engine
{

class EditorGui
{
public:
    bool Initialize(GLFWwindow* window);
    void Shutdown();

    void BeginFrame();
    void Draw(Scene& scene, float deltaTime);
    void EndFrame();

    bool WantsMouseCapture() const;
    bool WantsKeyboardCapture() const;

private:
    void DrawMenuBar(Scene& scene);
    void DrawHierarchy(Scene& scene);
    void DrawInspector(Scene& scene);
    void DrawStats(float deltaTime);
    void DrawControls();

    bool m_Initialized = false;
};

}
