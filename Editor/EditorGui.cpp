#include "Editor/EditorGui.h"

#include "Core/ConsoleLog.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cstdint>
#include <filesystem>
#include <string>
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

void EditorGui::Draw(Scene& scene, Renderer& renderer, float deltaTime, unsigned int viewportTextureId)
{
    m_EditingText = false;

    DrawMenuBar(scene);
    DrawToolbar();
    DrawViewport(viewportTextureId);
    HandleViewportMouse(scene);
    DrawHierarchy(scene);
    DrawInspector(scene);
    DrawStats(deltaTime);
    DrawLighting(renderer);
    DrawAssetBrowser();
    DrawConsole();
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

bool EditorGui::IsEditingText() const
{
    return m_EditingText;
}

float EditorGui::GetMouseWheel() const
{
    return ImGui::GetIO().MouseWheel;
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
                ConsoleLog::Info("Deleted selected object");
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

void EditorGui::DrawToolbar()
{
    ImGui::SetNextWindowPos(ImVec2(285.0f, 30.0f), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(680.0f, 45.0f), ImGuiCond_FirstUseEver);

    const ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoScrollbar;

    ImGui::Begin("Toolbar", nullptr, flags);

    ImGui::TextUnformatted("Tool:");
    ImGui::SameLine();

    if (ImGui::Button("Select"))
    {
        m_CurrentTool = EditorTool::Select;
    }

    ImGui::SameLine();

    if (ImGui::Button("Move"))
    {
        m_CurrentTool = EditorTool::Move;
    }

    ImGui::SameLine();

    if (ImGui::Button("Rotate"))
    {
        m_CurrentTool = EditorTool::Rotate;
    }

    ImGui::SameLine();

    if (ImGui::Button("Scale"))
    {
        m_CurrentTool = EditorTool::Scale;
    }

    ImGui::SameLine();

    if (ImGui::Button("Camera"))
    {
        m_CurrentTool = EditorTool::Camera;
    }

    ImGui::End();
}

void EditorGui::DrawViewport(unsigned int viewportTextureId)
{
    ImGui::SetNextWindowPos(ImVec2(285.0f, 85.0f), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(680.0f, 520.0f), ImGuiCond_FirstUseEver);

    const ImGuiWindowFlags viewportFlags =
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoScrollWithMouse;

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

        const ImVec2 min = ImGui::GetItemRectMin();
        const ImVec2 max = ImGui::GetItemRectMax();
        const ImVec2 center((min.x + max.x) * 0.5f, (min.y + max.y) * 0.5f);
        const float radius = available.x < available.y ? available.x * 0.18f : available.y * 0.18f;
        ImDrawList* drawList = ImGui::GetWindowDrawList();

        const ImU32 yellow = IM_COL32(255, 225, 30, 255);
        const ImU32 red = IM_COL32(255, 80, 80, 255);
        const ImU32 green = IM_COL32(80, 230, 120, 255);
        const ImU32 blue = IM_COL32(80, 180, 255, 255);
        const ImU32 white = IM_COL32(245, 245, 245, 255);
        const float arrow = 9.0f;

        const ImVec2 left(center.x - radius, center.y);
        const ImVec2 right(center.x + radius, center.y);
        const ImVec2 up(center.x, center.y - radius);
        const ImVec2 down(center.x, center.y + radius);

        if (m_CurrentTool == EditorTool::Move)
        {
            const ImVec2 zEnd(center.x + radius * 0.65f, center.y - radius * 0.65f);

            drawList->AddCircleFilled(center, 4.0f, yellow, 16);
            drawList->AddLine(center, right, red, 3.0f);
            drawList->AddLine(center, up, green, 3.0f);
            drawList->AddLine(center, zEnd, blue, 3.0f);

            drawList->AddTriangleFilled(right, ImVec2(right.x - arrow, right.y - arrow * 0.65f), ImVec2(right.x - arrow, right.y + arrow * 0.65f), red);
            drawList->AddTriangleFilled(up, ImVec2(up.x - arrow * 0.65f, up.y + arrow), ImVec2(up.x + arrow * 0.65f, up.y + arrow), green);
            drawList->AddTriangleFilled(zEnd, ImVec2(zEnd.x - arrow, zEnd.y), ImVec2(zEnd.x, zEnd.y + arrow), blue);

            drawList->AddText(ImVec2(right.x + 8.0f, right.y - 8.0f), red, "X");
            drawList->AddText(ImVec2(up.x + 8.0f, up.y - 18.0f), green, "Y");
            drawList->AddText(ImVec2(zEnd.x + 8.0f, zEnd.y - 8.0f), blue, "Z");
        }

        if (m_CurrentTool == EditorTool::Rotate)
        {
            drawList->AddCircle(center, radius, yellow, 64, 2.5f);

            drawList->AddLine(left, right, red, 2.5f);
            drawList->AddTriangleFilled(right, ImVec2(right.x - arrow, right.y - arrow * 0.65f), ImVec2(right.x - arrow, right.y + arrow * 0.65f), red);
            drawList->AddTriangleFilled(left, ImVec2(left.x + arrow, left.y - arrow * 0.65f), ImVec2(left.x + arrow, left.y + arrow * 0.65f), red);

            drawList->AddLine(up, down, blue, 2.5f);
            drawList->AddTriangleFilled(up, ImVec2(up.x - arrow * 0.65f, up.y + arrow), ImVec2(up.x + arrow * 0.65f, up.y + arrow), blue);
            drawList->AddTriangleFilled(down, ImVec2(down.x - arrow * 0.65f, down.y - arrow), ImVec2(down.x + arrow * 0.65f, down.y - arrow), blue);

            drawList->AddText(ImVec2(right.x + 8.0f, right.y - 18.0f), red, "+Y");
            drawList->AddText(ImVec2(left.x - 28.0f, left.y - 18.0f), red, "-Y");
            drawList->AddText(ImVec2(up.x + 8.0f, up.y - 18.0f), blue, "-X");
            drawList->AddText(ImVec2(down.x + 8.0f, down.y + 2.0f), blue, "+X");
            drawList->AddText(ImVec2(center.x - 32.0f, center.y - 8.0f), white, "Rotate");
        }

        if (m_CurrentTool == EditorTool::Scale)
        {
            drawList->AddRect(ImVec2(center.x - radius * 0.45f, center.y - radius * 0.45f), ImVec2(center.x + radius * 0.45f, center.y + radius * 0.45f), yellow, 0.0f, 0, 2.5f);
            drawList->AddLine(center, right, red, 3.0f);
            drawList->AddLine(center, up, green, 3.0f);
            drawList->AddLine(center, ImVec2(center.x + radius * 0.6f, center.y - radius * 0.6f), blue, 3.0f);
            drawList->AddRectFilled(ImVec2(right.x - 5.0f, right.y - 5.0f), ImVec2(right.x + 5.0f, right.y + 5.0f), red);
            drawList->AddRectFilled(ImVec2(up.x - 5.0f, up.y - 5.0f), ImVec2(up.x + 5.0f, up.y + 5.0f), green);
            drawList->AddRectFilled(ImVec2(center.x + radius * 0.6f - 5.0f, center.y - radius * 0.6f - 5.0f), ImVec2(center.x + radius * 0.6f + 5.0f, center.y - radius * 0.6f + 5.0f), blue);
            drawList->AddText(ImVec2(center.x - 24.0f, center.y - 8.0f), white, "Scale");
        }
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
        std::string label = objects[index].Name.empty() ? "Unnamed" : objects[index].Name;
        label += "##SceneObject" + std::to_string(index);

        if (ImGui::Selectable(label.c_str(), selected))
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

    const int selectedIndex = scene.GetSelectedIndex();
    if (m_NameEditIndex != selectedIndex)
    {
        m_NameEditIndex = selectedIndex;
        m_NameBuffer.fill(0);
        selected->Name.copy(m_NameBuffer.data(), m_NameBuffer.size() - 1);
    }

    if (ImGui::InputText("Name", m_NameBuffer.data(), m_NameBuffer.size()))
    {
        selected->Name = m_NameBuffer.data();
    }

    if (ImGui::IsItemActive())
    {
        m_EditingText = true;
    }

    if (ImGui::IsItemDeactivatedAfterEdit() && selected->Name.empty())
    {
        selected->Name = "Unnamed";
        m_NameBuffer.fill(0);
        selected->Name.copy(m_NameBuffer.data(), m_NameBuffer.size() - 1);
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

void EditorGui::DrawLighting(Renderer& renderer)
{
    ImGui::SetNextWindowPos(ImVec2(980.0f, 405.0f), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(290.0f, 170.0f), ImGuiCond_FirstUseEver);

    ImGui::Begin("Lighting");

    LightingSettings& lighting = renderer.GetLightingSettings();
    ImGui::DragFloat("Ambient", &lighting.AmbientStrength, 0.01f, 0.0f, 1.0f);
    ImGui::DragFloat("Intensity", &lighting.Intensity, 0.01f, 0.0f, 3.0f);
    ImGui::DragFloat3("Direction", &lighting.DirectionX, 0.02f, -1.0f, 1.0f);

    ImGui::End();
}

void EditorGui::DrawAssetBrowser()
{
    ImGui::SetNextWindowPos(ImVec2(405.0f, 615.0f), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(555.0f, 130.0f), ImGuiCond_FirstUseEver);

    ImGui::Begin("Asset Browser");

    const char* roots[] = {"Assets", "Shaders", "Screenshots", "Scenes"};
    for (const char* root : roots)
    {
        if (ImGui::TreeNode(root))
        {
            if (std::filesystem::exists(root))
            {
                for (const auto& entry : std::filesystem::directory_iterator(root))
                {
                    ImGui::TextUnformatted(entry.path().filename().string().c_str());
                }
            }
            else
            {
                ImGui::TextDisabled("Folder missing");
            }

            ImGui::TreePop();
        }
    }

    ImGui::End();
}

void EditorGui::DrawConsole()
{
    ImGui::SetNextWindowPos(ImVec2(405.0f, 750.0f), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(555.0f, 160.0f), ImGuiCond_FirstUseEver);

    ImGui::Begin("Console");

    if (ImGui::Button("Clear"))
    {
        ConsoleLog::Clear();
    }

    ImGui::Separator();

    for (const std::string& message : ConsoleLog::GetMessages())
    {
        ImGui::TextWrapped("%s", message.c_str());
    }

    ImGui::End();
}
void EditorGui::DrawControls()
{
    ImGui::SetNextWindowPos(ImVec2(10.0f, 490.0f), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(380.0f, 170.0f), ImGuiCond_FirstUseEver);

    ImGui::Begin("Controls");

    ImGui::TextUnformatted("Toolbar");
    ImGui::BulletText("Select: left mouse drag pans scene");
    ImGui::BulletText("Move: left drag X/Y, mouse wheel Z depth");
    ImGui::BulletText("Rotate: left drag X/Y, mouse wheel Z roll");
    ImGui::BulletText("Scale: left mouse drag up/down in Viewport");
    ImGui::BulletText("Viewport: hold right mouse to look around");
    ImGui::BulletText("Viewport: hold right mouse + WASD to fly");
    ImGui::BulletText("Viewport: middle drag to pan");
    ImGui::BulletText("Viewport: mouse wheel zooms in/out");
    ImGui::BulletText("Inspector: exact values");

    ImGui::End();
}

void EditorGui::HandleViewportMouse(Scene& scene)
{
    if (!m_ViewportHovered)
    {
        return;
    }

    SceneObject* selected = scene.GetSelectedObject();
    if (!selected)
    {
        return;
    }

    const ImVec2 delta = ImGui::GetIO().MouseDelta;
    const float wheel = ImGui::GetIO().MouseWheel;

    if (m_CurrentTool == EditorTool::Move)
    {
        if (ImGui::IsMouseDragging(ImGuiMouseButton_Left))
        {
            selected->TransformData.Position.x += delta.x * 0.01f;
            selected->TransformData.Position.y -= delta.y * 0.01f;
        }

        if (wheel != 0.0f)
        {
            selected->TransformData.Position.z += wheel * 0.25f;
        }
    }

    if (!ImGui::IsMouseDragging(ImGuiMouseButton_Left))
    {
        return;
    }

    if (m_CurrentTool == EditorTool::Rotate)
    {
        selected->TransformData.Rotation.y += delta.x * 0.01f;
        selected->TransformData.Rotation.x += delta.y * 0.01f;

        if (wheel != 0.0f)
        {
            selected->TransformData.Rotation.z += wheel * 0.12f;
        }
    }

    if (m_CurrentTool == EditorTool::Scale)
    {
        const float scaleDelta = -delta.y * 0.01f;
        selected->TransformData.Scale.x += scaleDelta;
        selected->TransformData.Scale.y += scaleDelta;
        selected->TransformData.Scale.z += scaleDelta;

        if (selected->TransformData.Scale.x < 0.1f)
        {
            selected->TransformData.Scale = {0.1f, 0.1f, 0.1f};
        }
    }
}

}


















