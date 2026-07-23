#pragma once

#include "Renderer/Material.h"
#include "Scene/Transform.h"

#include <string>
#include <vector>

namespace Engine
{

enum class SceneObjectType
{
    Cube,
    Circle,
    Character,
    Terrain
};

enum class CollisionShapeType
{
    None,
    Box,
    Terrain
};

struct SceneObject
{
    std::string Name;
    Transform TransformData;
    Material MaterialData;
    SceneObjectType Type = SceneObjectType::Cube;
    CollisionShapeType CollisionShape = CollisionShapeType::Box;
};

class Scene
{
public:
    Scene();

    void NewProject(const std::string& projectName = "Untitled Project", const std::string& sceneName = "Main Scene");
    void SetProjectName(const std::string& projectName);
    void SetSceneName(const std::string& sceneName);
    const std::string& GetProjectName() const;
    const std::string& GetSceneName() const;

    void AddCube();
    void AddCircle();
    void AddCharacter();
    void AddTerrain();
    void DeleteSelected();
    void SelectNext();
    void SelectPrevious();
    void Select(int index);
    bool SaveToFile(const std::string& path) const;
    bool LoadFromFile(const std::string& path);

    SceneObject* GetSelectedObject();
    const SceneObject* GetSelectedObject() const;
    std::vector<SceneObject>& GetObjects();
    const std::vector<SceneObject>& GetObjects() const;

    int GetSelectedIndex() const;

private:
    std::string m_ProjectName = "Untitled Project";
    std::string m_SceneName = "Main Scene";
    std::vector<SceneObject> m_Objects;
    int m_SelectedIndex = 0;
};

}



