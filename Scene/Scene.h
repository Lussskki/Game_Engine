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
    Terrain,
    TerrainWall
};

enum class CollisionShapeType
{
    None,
    Box,
    Terrain,
    Wall
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

    void AddCube();
    void AddTerrain();
    void AddTerrainWall();
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
    std::vector<SceneObject> m_Objects;
    int m_SelectedIndex = 0;
};

}
