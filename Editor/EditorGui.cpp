#include "Editor/EditorGui.h"

#include "Core/ConsoleLog.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <cstdint>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <string>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

namespace Engine
{
namespace
{

std::string MakeSafeFileName(const std::string& name, const std::string& fallback)
{
    std::string fileName = name;
    for (char& character : fileName)
    {
        const unsigned char value = static_cast<unsigned char>(character);
        if (!std::isalnum(value) && character != '-' && character != '_')
        {
            character = '_';
        }
    }

    if (fileName.empty())
    {
        fileName = fallback;
    }

    return fileName;
}

std::filesystem::path GetProjectSavePath(const Scene& scene)
{
    return std::filesystem::path("Scenes") / (MakeSafeFileName(scene.GetProjectName(), "Untitled_Project") + ".scene");
}

std::filesystem::path GetRunningExecutablePath()
{
#ifdef _WIN32
    char path[MAX_PATH] = {};
    const DWORD length = GetModuleFileNameA(nullptr, path, MAX_PATH);
    if (length > 0 && length < MAX_PATH)
    {
        return path;
    }
#endif

    return std::filesystem::current_path() / "Merso.exe";
}

std::filesystem::path GetPlayerExecutablePath()
{
    return GetRunningExecutablePath().parent_path() / "MersoPlayer.exe";
}

void SaveProject(Scene& scene)
{
    std::filesystem::create_directories("Scenes");
    scene.SaveToFile(GetProjectSavePath(scene).string());
}

void ExportProject(Scene& scene)
{
    SaveProject(scene);

    const std::string exportName = MakeSafeFileName(scene.GetProjectName(), "Untitled_Project");
    const std::filesystem::path exportRoot = std::filesystem::path("Exports") / exportName;
    const std::filesystem::path exportScene = exportRoot / "Scenes" / (exportName + ".scene");
    const std::filesystem::path exportExecutable = exportRoot / (exportName + ".exe");
    std::error_code error;

    if (std::filesystem::exists(exportRoot))
    {
        std::filesystem::remove_all(exportRoot, error);
        if (error)
        {
            ConsoleLog::Error("Failed to clean export folder: " + error.message());
            return;
        }
    }

    std::filesystem::create_directories(exportRoot / "Scenes", error);
    if (error)
    {
        ConsoleLog::Error("Failed to create export folder: " + exportRoot.string());
        return;
    }

    const std::filesystem::path playerExecutable = GetPlayerExecutablePath();
    if (!std::filesystem::exists(playerExecutable))
    {
        ConsoleLog::Error("Failed to export player: build MersoPlayer.exe first.");
        return;
    }

    std::filesystem::copy_file(playerExecutable, exportExecutable, std::filesystem::copy_options::overwrite_existing, error);
    if (error)
    {
        ConsoleLog::Error("Failed to export executable: " + error.message());
        return;
    }

    std::filesystem::copy("Shaders", exportRoot / "Shaders", std::filesystem::copy_options::recursive | std::filesystem::copy_options::overwrite_existing, error);
    if (error)
    {
        ConsoleLog::Error("Failed to export shaders: " + error.message());
        return;
    }

    std::filesystem::copy("Assets", exportRoot / "Assets", std::filesystem::copy_options::recursive | std::filesystem::copy_options::overwrite_existing, error);
    if (error)
    {
        ConsoleLog::Error("Failed to export assets: " + error.message());
        return;
    }

    std::filesystem::copy_file(GetProjectSavePath(scene), exportScene, std::filesystem::copy_options::overwrite_existing, error);
    if (error)
    {
        ConsoleLog::Error("Failed to export scene file: " + error.message());
        return;
    }

    std::ofstream launcher(exportRoot / "PLAY.bat");
    launcher << "@echo off\n";
    launcher << "cd /d \"%~dp0\"\n";
    launcher << "\"" << exportName << ".exe\"\n";

    ConsoleLog::Info("Exported project: " + exportRoot.string());
}

}

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

void EditorGui::Draw(Scene& scene, Renderer& renderer, const Input& input, float deltaTime, unsigned int viewportTextureId)
{
    m_EditingText = false;

    DrawMenuBar(scene);
    DrawToolbar();
    DrawViewport(scene, renderer, viewportTextureId);
    HandleViewportMouse(scene, renderer);
    DrawHierarchy(scene);
    DrawInspector(scene);
    DrawStats(deltaTime);
    DrawLighting(renderer);
    DrawConsole();
    DrawControls();

    HandleKeyboardShortcuts(scene, input);
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
            DrawProjectActionsMenu(scene);
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


        if (ImGui::BeginMenu("Models"))
        {
            if (ImGui::MenuItem("Circle"))
            {
                scene.AddCircle();
            }

            if (ImGui::MenuItem("Character"))
            {
                scene.AddCharacter();
            }

            if (ImGui::MenuItem("Cube"))
            {
                scene.AddCube();
            }

            if (ImGui::MenuItem("Terrain"))
            {
                scene.AddTerrain();
            }

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

    if (ImGui::Button(m_Playing ? "Pause" : "Play"))
    {
        m_Playing = !m_Playing;
        ConsoleLog::Info(m_Playing ? "Play mode started" : "Play mode paused");
    }

    ImGui::SameLine();
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

void EditorGui::DrawViewport(Scene& scene, Renderer& renderer, unsigned int viewportTextureId)
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
        m_ViewportImageMinX = min.x;
        m_ViewportImageMinY = min.y;

        ImVec2 center((min.x + max.x) * 0.5f, (min.y + max.y) * 0.5f);
        const SceneObject* selectedObject = scene.GetSelectedObject();
        if (selectedObject)
        {
            float projectedX = 0.0f;
            float projectedY = 0.0f;
            if (renderer.ProjectWorldToViewport(selectedObject->TransformData.Position, m_ViewportWidth, m_ViewportHeight, projectedX, projectedY))
            {
                center = ImVec2(min.x + projectedX, min.y + projectedY);
            }
        }

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

        auto projectAxisEnd = [&](const Vec3& axis, ImVec2 fallback)
        {
            if (!selectedObject)
            {
                return fallback;
            }

            const Vec3 origin = selectedObject->TransformData.Position;
            const Vec3 end = {
                origin.x + axis.x * 1.6f,
                origin.y + axis.y * 1.6f,
                origin.z + axis.z * 1.6f
            };

            float projectedX = 0.0f;
            float projectedY = 0.0f;
            if (!renderer.ProjectWorldToViewport(end, m_ViewportWidth, m_ViewportHeight, projectedX, projectedY))
            {
                return fallback;
            }

            return ImVec2(min.x + projectedX, min.y + projectedY);
        };

        if (m_CurrentTool == EditorTool::Move)
        {
            const ImVec2 xEnd = projectAxisEnd({1.0f, 0.0f, 0.0f}, right);
            const ImVec2 yEnd = projectAxisEnd({0.0f, 1.0f, 0.0f}, up);
            const ImVec2 zEnd = projectAxisEnd({0.0f, 0.0f, 1.0f}, ImVec2(center.x + radius * 0.65f, center.y - radius * 0.65f));

            drawList->AddCircleFilled(center, 4.0f, yellow, 16);
            drawList->AddLine(center, xEnd, m_ActiveMoveAxis == EditorAxis::X ? yellow : red, m_ActiveMoveAxis == EditorAxis::X ? 5.0f : 3.0f);
            drawList->AddLine(center, yEnd, m_ActiveMoveAxis == EditorAxis::Y ? yellow : green, m_ActiveMoveAxis == EditorAxis::Y ? 5.0f : 3.0f);
            drawList->AddLine(center, zEnd, m_ActiveMoveAxis == EditorAxis::Z ? yellow : blue, m_ActiveMoveAxis == EditorAxis::Z ? 5.0f : 3.0f);

            drawList->AddCircleFilled(xEnd, 5.0f, red, 16);
            drawList->AddCircleFilled(yEnd, 5.0f, green, 16);
            drawList->AddCircleFilled(zEnd, 5.0f, blue, 16);

            drawList->AddText(ImVec2(xEnd.x + 8.0f, xEnd.y - 8.0f), red, "X");
            drawList->AddText(ImVec2(yEnd.x + 8.0f, yEnd.y - 18.0f), green, "Y");
            drawList->AddText(ImVec2(zEnd.x + 8.0f, zEnd.y - 8.0f), blue, "Z");
        }

        if (m_CurrentTool == EditorTool::Rotate && selectedObject)
        {
            auto projectWorldPoint = [&](const Vec3& point, ImVec2& screenPoint)
            {
                float projectedX = 0.0f;
                float projectedY = 0.0f;
                if (!renderer.ProjectWorldToViewport(point, m_ViewportWidth, m_ViewportHeight, projectedX, projectedY))
                {
                    return false;
                }

                screenPoint = ImVec2(min.x + projectedX, min.y + projectedY);
                return true;
            };

            auto drawRotationRing = [&](EditorAxis axis, ImU32 color)
            {
                const Vec3 origin = selectedObject->TransformData.Position;
                const float ringRadius = 1.35f;
                const int segmentCount = 96;
                ImVec2 previousPoint;
                bool hasPreviousPoint = false;

                for (int segmentIndex = 0; segmentIndex <= segmentCount; segmentIndex++)
                {
                    const float angle = static_cast<float>(segmentIndex) / static_cast<float>(segmentCount) * 6.28318530718f;
                    const float c = std::cos(angle) * ringRadius;
                    const float s = std::sin(angle) * ringRadius;
                    Vec3 worldPoint = origin;

                    if (axis == EditorAxis::X)
                    {
                        worldPoint.y += c;
                        worldPoint.z += s;
                    }
                    else if (axis == EditorAxis::Y)
                    {
                        worldPoint.x += c;
                        worldPoint.z += s;
                    }
                    else
                    {
                        worldPoint.x += c;
                        worldPoint.y += s;
                    }

                    ImVec2 currentPoint;
                    if (!projectWorldPoint(worldPoint, currentPoint))
                    {
                        hasPreviousPoint = false;
                        continue;
                    }

                    if (hasPreviousPoint)
                    {
                        drawList->AddLine(previousPoint, currentPoint, m_ActiveRotateAxis == axis ? yellow : color, m_ActiveRotateAxis == axis ? 4.0f : 2.5f);
                    }

                    previousPoint = currentPoint;
                    hasPreviousPoint = true;
                }
            };

            drawList->AddCircle(center, radius * 0.75f, IM_COL32(230, 230, 230, 95), 96, 2.0f);
            drawRotationRing(EditorAxis::X, red);
            drawRotationRing(EditorAxis::Y, green);
            drawRotationRing(EditorAxis::Z, blue);
            drawList->AddCircleFilled(center, 4.0f, yellow, 16);
        }

        if (m_CurrentTool == EditorTool::Scale)
        {
            if (selectedObject && (selectedObject->Type == SceneObjectType::Terrain))
            {
                const float halfSize = 10.0f;
                const Vec3 localCorners[] = {
                    {-halfSize, 0.0f, -halfSize},
                    { halfSize, 0.0f, -halfSize},
                    { halfSize, 0.0f,  halfSize},
                    {-halfSize, 0.0f,  halfSize}
                };
                const auto model = selectedObject->TransformData.GetMatrix();
                Vec3 worldCorners[4];
                for (int cornerIndex = 0; cornerIndex < 4; cornerIndex++)
                {
                    const Vec3& local = localCorners[cornerIndex];
                    worldCorners[cornerIndex] = {
                        model[0] * local.x + model[4] * local.y + model[8] * local.z + model[12],
                        model[1] * local.x + model[5] * local.y + model[9] * local.z + model[13],
                        model[2] * local.x + model[6] * local.y + model[10] * local.z + model[14]
                    };
                }

                ImVec2 projectedCorners[4];
                bool allVisible = true;
                for (int cornerIndex = 0; cornerIndex < 4; cornerIndex++)
                {
                    float projectedX = 0.0f;
                    float projectedY = 0.0f;
                    if (!renderer.ProjectWorldToViewport(worldCorners[cornerIndex], m_ViewportWidth, m_ViewportHeight, projectedX, projectedY))
                    {
                        allVisible = false;
                        break;
                    }

                    projectedCorners[cornerIndex] = ImVec2(min.x + projectedX, min.y + projectedY);
                }

                if (allVisible)
                {
                    for (int cornerIndex = 0; cornerIndex < 4; cornerIndex++)
                    {
                        const ImVec2 start = projectedCorners[cornerIndex];
                        const ImVec2 end = projectedCorners[(cornerIndex + 1) % 4];
                        drawList->AddLine(start, end, yellow, 3.0f);
                        drawList->AddRectFilled(ImVec2(start.x - 6.0f, start.y - 6.0f), ImVec2(start.x + 6.0f, start.y + 6.0f), yellow);
                    }
                }

                drawList->AddText(ImVec2(center.x + 12.0f, center.y - 8.0f), white, "Resize Terrain");
            }
            else
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
    }

    ImGui::End();
}

void EditorGui::DrawHierarchy(Scene& scene)
{
    ImGui::SetNextWindowPos(ImVec2(10.0f, 30.0f), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(260.0f, 320.0f), ImGuiCond_FirstUseEver);

    ImGui::Begin("Scene Hierarchy");

    if (ImGui::BeginPopupContextWindow("SceneHierarchyContext", ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems))
    {
        DrawProjectActionsMenu(scene);
        ImGui::EndPopup();
    }

    const ImGuiTreeNodeFlags rootFlags =
        ImGuiTreeNodeFlags_DefaultOpen |
        ImGuiTreeNodeFlags_OpenOnArrow |
        ImGuiTreeNodeFlags_SpanAvailWidth;

    const std::string rootLabel = scene.GetProjectName() + "##ProjectRoot";
    if (ImGui::TreeNodeEx(rootLabel.c_str(), rootFlags))
    {
        const std::string sceneLabel = scene.GetSceneName() + "##SceneRoot";
        if (ImGui::TreeNodeEx(sceneLabel.c_str(), rootFlags))
        {
            const auto& objects = scene.GetObjects();
            for (int index = 0; index < static_cast<int>(objects.size()); index++)
            {
                const bool selected = index == scene.GetSelectedIndex();
                std::string label = objects[index].Name.empty() ? "Unnamed" : objects[index].Name;
                label += "##SceneObject" + std::to_string(index);

                ImGuiTreeNodeFlags objectFlags =
                    ImGuiTreeNodeFlags_DefaultOpen |
                    ImGuiTreeNodeFlags_OpenOnArrow |
                    ImGuiTreeNodeFlags_SpanAvailWidth;

                if (selected)
                {
                    objectFlags |= ImGuiTreeNodeFlags_Selected;
                }

                const bool open = ImGui::TreeNodeEx(label.c_str(), objectFlags);
                if (ImGui::IsItemClicked())
                {
                    scene.Select(index);
                }

                if (open)
                {
                    const char* collisionText = "Collision: None";
                    if (objects[index].CollisionShape == CollisionShapeType::Box)
                    {
                        collisionText = "Collision: Box";
                    }
                    else if (objects[index].CollisionShape == CollisionShapeType::Terrain)
                    {
                        collisionText = "Collision: Terrain";
                    }

                    ImGui::Indent(12.0f);
                    ImGui::TextDisabled("%s", collisionText);
                    ImGui::Unindent(12.0f);
                    ImGui::TreePop();
                }
            }

            ImGui::TreePop();
        }

        ImGui::TreePop();
    }

    ImGui::End();
}

void EditorGui::DrawProjectActionsMenu(Scene& scene)
{
    if (ImGui::MenuItem("New Project"))
    {
        scene.NewProject();
        ConsoleLog::Info("Created new project");
    }

    ImGui::Separator();

    const std::string& projectName = scene.GetProjectName();
    if (projectName != m_ProjectNameBuffer.data())
    {
        m_ProjectNameBuffer.fill(0);
        projectName.copy(m_ProjectNameBuffer.data(), m_ProjectNameBuffer.size() - 1);
    }

    ImGui::TextUnformatted("Project");
    ImGui::SetNextItemWidth(180.0f);
    if (ImGui::InputText("##ProjectNameMenu", m_ProjectNameBuffer.data(), m_ProjectNameBuffer.size()))
    {
        scene.SetProjectName(m_ProjectNameBuffer.data());
    }

    if (ImGui::IsItemActive())
    {
        m_EditingText = true;
    }

    const std::string& sceneName = scene.GetSceneName();
    if (sceneName != m_SceneNameBuffer.data())
    {
        m_SceneNameBuffer.fill(0);
        sceneName.copy(m_SceneNameBuffer.data(), m_SceneNameBuffer.size() - 1);
    }

    ImGui::TextUnformatted("Scene");
    ImGui::SetNextItemWidth(180.0f);
    if (ImGui::InputText("##SceneNameMenu", m_SceneNameBuffer.data(), m_SceneNameBuffer.size()))
    {
        scene.SetSceneName(m_SceneNameBuffer.data());
    }

    if (ImGui::IsItemActive())
    {
        m_EditingText = true;
    }

    if (ImGui::MenuItem("Save Project", "Ctrl+S"))
    {
        SaveProject(scene);
    }

    if (ImGui::MenuItem("Export Project"))
    {
        ExportProject(scene);
    }

    ImGui::Separator();

    if (ImGui::MenuItem("New Cube", "Ctrl+N"))
    {
        scene.AddCube();
    }

    if (ImGui::MenuItem("New Circle"))
    {
        scene.AddCircle();
    }

    if (ImGui::MenuItem("New Character"))
    {
        scene.AddCharacter();
    }

    if (ImGui::MenuItem("New Terrain"))
    {
        scene.AddTerrain();
    }

    if (ImGui::MenuItem("Delete Selected", "Ctrl+Del"))
    {
        scene.DeleteSelected();
        ConsoleLog::Info("Deleted selected object");
    }
}

void EditorGui::HandleKeyboardShortcuts(Scene& scene, const Input& input)
{
    const bool controlPressed = input.IsKeyPressed(GLFW_KEY_LEFT_CONTROL) || input.IsKeyPressed(GLFW_KEY_RIGHT_CONTROL);

    if (controlPressed && input.IsKeyJustPressed(GLFW_KEY_S))
    {
        SaveProject(scene);
        return;
    }

    if (m_EditingText || ImGui::GetIO().WantTextInput)
    {
        return;
    }

    if (controlPressed && input.IsKeyJustPressed(GLFW_KEY_N))
    {
        scene.AddCube();
        return;
    }

    if (controlPressed && input.IsKeyJustPressed(GLFW_KEY_DELETE))
    {
        scene.DeleteSelected();
        ConsoleLog::Info("Deleted selected object");
    }
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

    const char* typeText = "Cube";
    if (selected->Type == SceneObjectType::Circle)
    {
        typeText = "Circle";
    }
    else if (selected->Type == SceneObjectType::Character)
    {
        typeText = "Character";
    }
    else if (selected->Type == SceneObjectType::Terrain)
    {
        typeText = "Terrain";
    }
    ImGui::Text("Type: %s", typeText);

    int collisionIndex = 0;
    if (selected->CollisionShape == CollisionShapeType::Box)
    {
        collisionIndex = 1;
    }
    else if (selected->CollisionShape == CollisionShapeType::Terrain)
    {
        collisionIndex = 2;
    }

    const char* collisionItems[] = {"None", "Box", "Terrain"};
    if (ImGui::Combo("Collision Shape", &collisionIndex, collisionItems, 3))
    {
        if (collisionIndex == 1)
        {
            selected->CollisionShape = CollisionShapeType::Box;
        }
        else if (collisionIndex == 2)
        {
            selected->CollisionShape = CollisionShapeType::Terrain;
        }
        else
        {
            selected->CollisionShape = CollisionShapeType::None;
        }
    }

    ImGui::Separator();

    ImGui::DragFloat3("Position", &selected->TransformData.Position.x, 0.05f);
    ImGui::DragFloat3("Rotation", &selected->TransformData.Rotation.x, 0.02f);

    if (selected->Type == SceneObjectType::Terrain)
    {
        constexpr float HalfPi = 1.57079632679f;

        int terrainDirection = 0;
        if (std::abs(selected->TransformData.Rotation.x - HalfPi) < 0.1f)
        {
            terrainDirection = 1;
        }
        else if (std::abs(selected->TransformData.Rotation.z - HalfPi) < 0.1f)
        {
            terrainDirection = 2;
        }

        const char* terrainDirectionItems[] = {"Flat Floor", "Wall Across X", "Wall Across Z"};
        if (ImGui::Combo("Terrain Direction", &terrainDirection, terrainDirectionItems, 3))
        {
            selected->TransformData.Rotation.x = 0.0f;
            selected->TransformData.Rotation.y = 0.0f;
            selected->TransformData.Rotation.z = 0.0f;

            if (terrainDirection == 1)
            {
                selected->TransformData.Rotation.x = HalfPi;
            }
            else if (terrainDirection == 2)
            {
                selected->TransformData.Rotation.z = HalfPi;
            }
        }

        ImGui::TextUnformatted("Terrain Size");
        if (terrainDirection == 0)
        {
            ImGui::DragFloat("Width X", &selected->TransformData.Scale.x, 0.05f, 0.25f, 20.0f);
            ImGui::DragFloat("Depth Z", &selected->TransformData.Scale.z, 0.05f, 0.25f, 20.0f);
        }
        else if (terrainDirection == 1)
        {
            ImGui::DragFloat("Width X", &selected->TransformData.Scale.x, 0.05f, 0.25f, 20.0f);
            ImGui::DragFloat("Height Y", &selected->TransformData.Scale.z, 0.05f, 0.25f, 20.0f);
        }
        else
        {
            ImGui::DragFloat("Width Z", &selected->TransformData.Scale.z, 0.05f, 0.25f, 20.0f);
            ImGui::DragFloat("Height Y", &selected->TransformData.Scale.x, 0.05f, 0.25f, 20.0f);
        }

        selected->TransformData.Scale.y = 1.0f;
    }
    else
    {
        ImGui::DragFloat3("Scale", &selected->TransformData.Scale.x, 0.02f, 0.1f, 10.0f);
    }

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

    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.16f, 0.06f, 0.06f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.30f, 0.08f, 0.08f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.42f, 0.08f, 0.08f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_SliderGrab, ImVec4(0.95f, 0.12f, 0.10f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, ImVec4(1.0f, 0.30f, 0.22f, 1.0f));

    ImGui::TextUnformatted("Ambient - base light in shadows");
    ImGui::SliderFloat("##AmbientLight", &lighting.AmbientStrength, 0.0f, 1.0f, "");

    ImGui::TextUnformatted("Intensity - sun brightness");
    ImGui::SliderFloat("##LightIntensity", &lighting.Intensity, 0.0f, 3.0f, "");

    ImGui::TextUnformatted("Direction - sun angle X / Y / Z");
    ImGui::SliderFloat3("##LightDirection", &lighting.DirectionX, -1.0f, 1.0f, "");

    ImGui::Checkbox("Shadows", &lighting.ShadowsEnabled);
    ImGui::TextUnformatted("Shadow Strength");
    ImGui::SliderFloat("##ShadowStrength", &lighting.ShadowStrength, 0.0f, 0.85f, "");

    ImGui::PopStyleColor(5);

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
    ImGui::BulletText("Select: left drag selected object to place it");
    ImGui::BulletText("Move: left drag X/Y, mouse wheel Z depth");
    ImGui::BulletText("Rotate: left drag X/Y, mouse wheel Z roll");
    ImGui::BulletText("Scale: drag up/down; terrain resizes");
    ImGui::BulletText("Viewport: hold right mouse to look around");
    ImGui::BulletText("Viewport: hold right mouse + WASD to fly");
    ImGui::BulletText("Viewport: middle mouse drag pans scene");
    ImGui::BulletText("Viewport: mouse wheel zooms in/out");
    ImGui::BulletText("Play: WASD moves Character, Space jumps");
    ImGui::BulletText("Inspector: exact values and terrain direction");

    ImGui::End();
}

void EditorGui::HandleViewportMouse(Scene& scene, Renderer& renderer)
{
    if (!m_ViewportHovered)
    {
        return;
    }

    if (m_CurrentTool == EditorTool::Select && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
    {
        const ImVec2 mouse = ImGui::GetMousePos();
        const float localMouseX = mouse.x - m_ViewportImageMinX;
        const float localMouseY = mouse.y - m_ViewportImageMinY;
        int bestIndex = -1;
        float bestDistanceSquared = 2500.0f;

        const auto& objects = scene.GetObjects();
        for (int index = 0; index < static_cast<int>(objects.size()); index++)
        {
            float projectedX = 0.0f;
            float projectedY = 0.0f;
            if (!renderer.ProjectWorldToViewport(objects[index].TransformData.Position, m_ViewportWidth, m_ViewportHeight, projectedX, projectedY))
            {
                continue;
            }

            const float deltaX = projectedX - localMouseX;
            const float deltaY = projectedY - localMouseY;
            const float distanceSquared = deltaX * deltaX + deltaY * deltaY;
            if (distanceSquared < bestDistanceSquared)
            {
                bestDistanceSquared = distanceSquared;
                bestIndex = index;
            }
        }

        if (bestIndex >= 0)
        {
            scene.Select(bestIndex);
        }
    }

    SceneObject* selected = scene.GetSelectedObject();
    if (!selected)
    {
        return;
    }

    const ImVec2 delta = ImGui::GetIO().MouseDelta;
    const float wheel = ImGui::GetIO().MouseWheel;

    if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
    {
        m_ActiveMoveAxis = EditorAxis::None;
        m_ActiveRotateAxis = EditorAxis::None;
        m_DraggingSelectedObject = false;
    }

    if (m_CurrentTool == EditorTool::Select)
    {
        const ImVec2 mouse = ImGui::GetMousePos();
        const float localMouseX = mouse.x - m_ViewportImageMinX;
        const float localMouseY = mouse.y - m_ViewportImageMinY;
        Vec3 hitPoint;

        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
        {
            if (renderer.ViewportPointToPlane(localMouseX, localMouseY, m_ViewportWidth, m_ViewportHeight, selected->TransformData.Position.y, hitPoint))
            {
                m_SelectedDragOffset = {
                    selected->TransformData.Position.x - hitPoint.x,
                    0.0f,
                    selected->TransformData.Position.z - hitPoint.z
                };
                m_DraggingSelectedObject = true;
            }
        }

        if (m_DraggingSelectedObject && ImGui::IsMouseDragging(ImGuiMouseButton_Left))
        {
            if (renderer.ViewportPointToPlane(localMouseX, localMouseY, m_ViewportWidth, m_ViewportHeight, selected->TransformData.Position.y, hitPoint))
            {
                selected->TransformData.Position.x = hitPoint.x + m_SelectedDragOffset.x;
                selected->TransformData.Position.z = hitPoint.z + m_SelectedDragOffset.z;
            }
        }

        return;
    }
    if (m_CurrentTool == EditorTool::Move)
    {
        auto distanceToSegmentSquared = [](ImVec2 point, ImVec2 start, ImVec2 end)
        {
            const float segmentX = end.x - start.x;
            const float segmentY = end.y - start.y;
            const float lengthSquared = segmentX * segmentX + segmentY * segmentY;
            float t = 0.0f;
            if (lengthSquared > 0.0001f)
            {
                t = ((point.x - start.x) * segmentX + (point.y - start.y) * segmentY) / lengthSquared;
            }

            if (t < 0.0f)
            {
                t = 0.0f;
            }
            else if (t > 1.0f)
            {
                t = 1.0f;
            }

            const float closestX = start.x + segmentX * t;
            const float closestY = start.y + segmentY * t;
            const float deltaX = point.x - closestX;
            const float deltaY = point.y - closestY;
            return deltaX * deltaX + deltaY * deltaY;
        };

        auto projectAxis = [&](const Vec3& axis, ImVec2& center, ImVec2& end)
        {
            float centerX = 0.0f;
            float centerY = 0.0f;
            if (!renderer.ProjectWorldToViewport(selected->TransformData.Position, m_ViewportWidth, m_ViewportHeight, centerX, centerY))
            {
                return false;
            }

            const Vec3 axisEnd = {
                selected->TransformData.Position.x + axis.x * 1.6f,
                selected->TransformData.Position.y + axis.y * 1.6f,
                selected->TransformData.Position.z + axis.z * 1.6f
            };

            float endX = 0.0f;
            float endY = 0.0f;
            if (!renderer.ProjectWorldToViewport(axisEnd, m_ViewportWidth, m_ViewportHeight, endX, endY))
            {
                return false;
            }

            center = ImVec2(m_ViewportImageMinX + centerX, m_ViewportImageMinY + centerY);
            end = ImVec2(m_ViewportImageMinX + endX, m_ViewportImageMinY + endY);
            return true;
        };

        auto dragAmountOnAxis = [&](const Vec3& axis)
        {
            ImVec2 center;
            ImVec2 end;
            if (!projectAxis(axis, center, end))
            {
                return 0.0f;
            }

            const float axisX = end.x - center.x;
            const float axisY = end.y - center.y;
            const float length = std::sqrt(axisX * axisX + axisY * axisY);
            if (length <= 0.0001f)
            {
                return 0.0f;
            }

            const float normalizedX = axisX / length;
            const float normalizedY = axisY / length;
            return (delta.x * normalizedX + delta.y * normalizedY) * 0.015f;
        };

        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
        {
            const ImVec2 mouse = ImGui::GetMousePos();
            ImVec2 center;
            ImVec2 xEnd;
            ImVec2 yEnd;
            ImVec2 zEnd;
            const bool hasX = projectAxis({1.0f, 0.0f, 0.0f}, center, xEnd);
            const bool hasY = projectAxis({0.0f, 1.0f, 0.0f}, center, yEnd);
            const bool hasZ = projectAxis({0.0f, 0.0f, 1.0f}, center, zEnd);
            const float hitDistance = 576.0f;

            const float xDistance = hasX ? distanceToSegmentSquared(mouse, center, xEnd) : 999999.0f;
            const float yDistance = hasY ? distanceToSegmentSquared(mouse, center, yEnd) : 999999.0f;
            const float zDistance = hasZ ? distanceToSegmentSquared(mouse, center, zEnd) : 999999.0f;

            m_ActiveMoveAxis = EditorAxis::None;
            if (xDistance <= hitDistance && xDistance <= yDistance && xDistance <= zDistance)
            {
                m_ActiveMoveAxis = EditorAxis::X;
            }
            else if (yDistance <= hitDistance && yDistance <= zDistance)
            {
                m_ActiveMoveAxis = EditorAxis::Y;
            }
            else if (zDistance <= hitDistance)
            {
                m_ActiveMoveAxis = EditorAxis::Z;
            }
        }

        if (ImGui::IsMouseDragging(ImGuiMouseButton_Left))
        {
            if (m_ActiveMoveAxis == EditorAxis::X)
            {
                selected->TransformData.Position.x += dragAmountOnAxis({1.0f, 0.0f, 0.0f});
            }
            else if (m_ActiveMoveAxis == EditorAxis::Y)
            {
                selected->TransformData.Position.y += dragAmountOnAxis({0.0f, 1.0f, 0.0f});
            }
            else if (m_ActiveMoveAxis == EditorAxis::Z)
            {
                selected->TransformData.Position.z += dragAmountOnAxis({0.0f, 0.0f, 1.0f});
            }
        }

        if (wheel != 0.0f)
        {
            selected->TransformData.Position.z += wheel * 0.25f;
        }
    }

    if (m_CurrentTool == EditorTool::Rotate)
    {
        auto distanceToSegmentSquared = [](ImVec2 point, ImVec2 start, ImVec2 end)
        {
            const float segmentX = end.x - start.x;
            const float segmentY = end.y - start.y;
            const float lengthSquared = segmentX * segmentX + segmentY * segmentY;
            float t = 0.0f;
            if (lengthSquared > 0.0001f)
            {
                t = ((point.x - start.x) * segmentX + (point.y - start.y) * segmentY) / lengthSquared;
            }

            if (t < 0.0f)
            {
                t = 0.0f;
            }
            else if (t > 1.0f)
            {
                t = 1.0f;
            }

            const float closestX = start.x + segmentX * t;
            const float closestY = start.y + segmentY * t;
            const float deltaX = point.x - closestX;
            const float deltaY = point.y - closestY;
            return deltaX * deltaX + deltaY * deltaY;
        };

        auto projectWorldPoint = [&](const Vec3& point, ImVec2& screenPoint)
        {
            float projectedX = 0.0f;
            float projectedY = 0.0f;
            if (!renderer.ProjectWorldToViewport(point, m_ViewportWidth, m_ViewportHeight, projectedX, projectedY))
            {
                return false;
            }

            screenPoint = ImVec2(m_ViewportImageMinX + projectedX, m_ViewportImageMinY + projectedY);
            return true;
        };

        auto closestRingDistance = [&](EditorAxis axis, ImVec2 mouse)
        {
            const Vec3 origin = selected->TransformData.Position;
            const float ringRadius = 1.35f;
            const int segmentCount = 72;
            float closestDistance = 999999.0f;
            ImVec2 previousPoint;
            bool hasPreviousPoint = false;

            for (int segmentIndex = 0; segmentIndex <= segmentCount; segmentIndex++)
            {
                const float angle = static_cast<float>(segmentIndex) / static_cast<float>(segmentCount) * 6.28318530718f;
                const float c = std::cos(angle) * ringRadius;
                const float s = std::sin(angle) * ringRadius;
                Vec3 worldPoint = origin;

                if (axis == EditorAxis::X)
                {
                    worldPoint.y += c;
                    worldPoint.z += s;
                }
                else if (axis == EditorAxis::Y)
                {
                    worldPoint.x += c;
                    worldPoint.z += s;
                }
                else
                {
                    worldPoint.x += c;
                    worldPoint.y += s;
                }

                ImVec2 currentPoint;
                if (!projectWorldPoint(worldPoint, currentPoint))
                {
                    hasPreviousPoint = false;
                    continue;
                }

                if (hasPreviousPoint)
                {
                    const float distance = distanceToSegmentSquared(mouse, previousPoint, currentPoint);
                    if (distance < closestDistance)
                    {
                        closestDistance = distance;
                    }
                }

                previousPoint = currentPoint;
                hasPreviousPoint = true;
            }

            return closestDistance;
        };

        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
        {
            const ImVec2 mouse = ImGui::GetMousePos();
            const float xDistance = closestRingDistance(EditorAxis::X, mouse);
            const float yDistance = closestRingDistance(EditorAxis::Y, mouse);
            const float zDistance = closestRingDistance(EditorAxis::Z, mouse);
            const float hitDistance = 625.0f;

            m_ActiveRotateAxis = EditorAxis::None;
            if (xDistance <= hitDistance && xDistance <= yDistance && xDistance <= zDistance)
            {
                m_ActiveRotateAxis = EditorAxis::X;
            }
            else if (yDistance <= hitDistance && yDistance <= zDistance)
            {
                m_ActiveRotateAxis = EditorAxis::Y;
            }
            else if (zDistance <= hitDistance)
            {
                m_ActiveRotateAxis = EditorAxis::Z;
            }
        }

        if (ImGui::IsMouseDragging(ImGuiMouseButton_Left) && m_ActiveRotateAxis != EditorAxis::None)
        {
            ImVec2 center;
            if (projectWorldPoint(selected->TransformData.Position, center))
            {
                const ImVec2 currentMouse = ImGui::GetMousePos();
                const ImVec2 previousMouse(currentMouse.x - delta.x, currentMouse.y - delta.y);
                const float currentAngle = std::atan2(currentMouse.y - center.y, currentMouse.x - center.x);
                const float previousAngle = std::atan2(previousMouse.y - center.y, previousMouse.x - center.x);
                float angleDelta = currentAngle - previousAngle;

                if (angleDelta > 3.14159265359f)
                {
                    angleDelta -= 6.28318530718f;
                }
                else if (angleDelta < -3.14159265359f)
                {
                    angleDelta += 6.28318530718f;
                }

                if (m_ActiveRotateAxis == EditorAxis::X)
                {
                    selected->TransformData.Rotation.x += angleDelta;
                }
                else if (m_ActiveRotateAxis == EditorAxis::Y)
                {
                    selected->TransformData.Rotation.y += angleDelta;
                }
                else if (m_ActiveRotateAxis == EditorAxis::Z)
                {
                    selected->TransformData.Rotation.z += angleDelta;
                }
            }
        }
    }

    if (!ImGui::IsMouseDragging(ImGuiMouseButton_Left))
    {
        return;
    }

    if (m_CurrentTool == EditorTool::Scale)
    {
        const float scaleDelta = -delta.y * 0.01f;

        if ((selected->Type == SceneObjectType::Terrain))
        {
            selected->TransformData.Scale.x += scaleDelta;
            selected->TransformData.Scale.z += scaleDelta;
            selected->TransformData.Scale.y = 1.0f;

            if (selected->TransformData.Scale.x < 0.25f)
            {
                selected->TransformData.Scale.x = 0.25f;
            }

            if (selected->TransformData.Scale.z < 0.25f)
            {
                selected->TransformData.Scale.z = 0.25f;
            }
        }
        else
        {
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

}






































