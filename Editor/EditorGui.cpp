#include "Editor/EditorGui.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cstdint>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

namespace Engine
{

bool EditorGui::Initialize(GLFWwindow* window)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();

    if (!ImGui_ImplGlfw_InitForOpenGL(window, true))
    {
        return false;
    }

    if (!ImGui_ImplOpenGL3_Init("#version 330"))
    {
        return false;
    }

    m_Initialized = true;
    return true;
}

void EditorGui::Shutdown()
{
    if (!m_Initialized)
    {
        return;
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    m_Initialized = false;
}

void EditorGui::BeginFrame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void EditorGui::Draw(Scene& scene, float deltaTime, unsigned int viewportTextureId)
{
    DrawMenuBar(scene);
    DrawViewport(viewportTextureId);
    DrawHierarchy(scene);
    DrawInspector(scene);
    DrawStats(deltaTime);
    DrawControls();
}

void EditorGui::EndFrame()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

bool EditorGui::WantsMouseCapture() const
{
    return ImGui::GetIO().WantCaptureMouse;
}

bool EditorGui::WantsKeyboardCapture() const
{
    return ImGui::GetIO().WantCaptureKeyboard;
}

bool EditorGui::WantsTextInput() const
{
    return ImGui::GetIO().WantTextInput;
}

void EditorGui::DrawMenuBar(Scene& scene)
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("New Cube", "N"))
            {
                scene.AddCube();
            }

            if (ImGui::MenuItem("Delete Selected", "Del"))
            {
                scene.DeleteSelected();
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View"))
        {
            ImGui::MenuItem("Viewport", nullptr, true, false);
            ImGui::MenuItem("Scene Hierarchy", nullptr, true, false);
            ImGui::MenuItem("Inspector", nullptr, true, false);
            ImGui::MenuItem("Stats", nullptr, true, false);
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}

void EditorGui::DrawViewport(unsigned int viewportTextureId)
{
    ImGui::SetNextWindowPos(ImVec2(285.0f, 30.0f), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(680.0f, 520.0f), ImGuiCond_FirstUseEver);

    const ImGuiWindowFlags viewportFlags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
    ImGui::Begin("Viewport", nullptr, viewportFlags);

    m_ViewportHovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem);
    m_ViewportFocused = ImGui::IsWindowFocused();

    ImVec2 available = ImGui::GetContentRegionAvail();
    if (available.x > 1.0f && available.y > 1.0f)
    {
        m_ViewportWidth = static_cast<int>(available.x);
        m_ViewportHeight = static_cast<int>(available.y);

        ImTextureID textureId = static_cast<ImTextureID>(viewportTextureId);
        ImGui::Image(textureId, available, ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));
    }

    ImGui::End();
}

void EditorGui::DrawHierarchy(Scene& scene)
{
    ImGui::SetNextWindowPos(ImVec2(10.0f, 30.0f), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(260.0f, 320.0f), ImGuiCond_FirstUseEver);

    ImGui::Begin("Scene Hierarchy");

    if (ImGui::Button("Add Cube"))
    {
        scene.AddCube();
    }

    ImGui::SameLine();

    if (ImGui::Button("Delete"))
    {
        scene.DeleteSelected();
    }

    ImGui::Separator();

    const auto& objects = scene.GetObjects();
    for (int index = 0; index < static_cast<int>(objects.size()); index++)
    {
        const bool selected = index == scene.GetSelectedIndex();
        if (ImGui::Selectable(objects[index].Name.c_str(), selected))
        {
            scene.Select(index);
        }
    }

    ImGui::End();
}

void EditorGui::DrawInspector(Scene& scene)
{
    ImGui::SetNextWindowPos(ImVec2(980.0f, 30.0f), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(290.0f, 360.0f), ImGuiCond_FirstUseEver);

    ImGui::Begin("Inspector");

    SceneObject* selected = scene.GetSelectedObject();
    if (!selected)
    {
        ImGui::TextUnformatted("No object selected.");
        ImGui::End();
        return;
    }

    char nameBuffer[128] = {};
    selected->Name.copy(nameBuffer, sizeof(nameBuffer) - 1);
    if (ImGui::InputText("Name", nameBuffer, sizeof(nameBuffer)))
    {
        selected->Name = nameBuffer;
    }

    ImGui::Separator();

    ImGui::DragFloat3("Position", &selected->TransformData.Position.x, 0.05f);
    ImGui::DragFloat3("Rotation", &selected->TransformData.Rotation.x, 0.02f);
    ImGui::DragFloat3("Scale", &selected->TransformData.Scale.x, 0.02f, 0.1f, 10.0f);

    ImGui::End();
}

void EditorGui::DrawStats(float deltaTime)
{
    ImGui::SetNextWindowPos(ImVec2(10.0f, 360.0f), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(260.0f, 120.0f), ImGuiCond_FirstUseEver);

    ImGui::Begin("Stats");

    const float fps = deltaTime > 0.0f ? 1.0f / deltaTime : 0.0f;
    ImGui::Text("FPS: %.1f", fps);
    ImGui::Text("Frame: %.3f ms", deltaTime * 1000.0f);
    ImGui::Text("Viewport: %d x %d", m_ViewportWidth, m_ViewportHeight);
    ImGui::Text("Viewport hovered: %s", m_ViewportHovered ? "yes" : "no");
    ImGui::TextUnformatted("Renderer: OpenGL");

    ImGui::End();
}

void EditorGui::DrawControls()
{
    ImGui::SetNextWindowPos(ImVec2(10.0f, 490.0f), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(380.0f, 170.0f), ImGuiCond_FirstUseEver);

    ImGui::Begin("Controls");

    ImGui::TextUnformatted("Camera");
    ImGui::BulletText("Hover Viewport, then WASD: move");
    ImGui::BulletText("Hover Viewport, then Space/C: up/down");
    ImGui::BulletText("Hover Viewport, then right mouse drag: look around");

    ImGui::Separator();

    ImGui::TextUnformatted("Object Editing");
    ImGui::BulletText("IJKL/UO: move selected object");
    ImGui::BulletText("Left mouse drag or arrow keys: rotate selected object");
    ImGui::BulletText("Z/X: scale selected object");

    ImGui::End();
}

}

