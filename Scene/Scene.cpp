#include "Scene/Scene.h"

#include "Core/ConsoleLog.h"

#include <algorithm>
#include <fstream>

namespace Engine
{

namespace
{

int ObjectTypeToInt(SceneObjectType type)
{
    if (type == SceneObjectType::Terrain)
    {
        return 1;
    }

    if (type == SceneObjectType::TerrainWall)
    {
        return 2;
    }

    return 0;
}

int CollisionShapeToInt(CollisionShapeType shape)
{
    if (shape == CollisionShapeType::Box)
    {
        return 1;
    }

    if (shape == CollisionShapeType::Terrain)
    {
        return 2;
    }

    if (shape == CollisionShapeType::Wall)
    {
        return 3;
    }

    return 0;
}

SceneObjectType ObjectTypeFromInt(int value)
{
    if (value == 1)
    {
        return SceneObjectType::Terrain;
    }

    if (value == 2)
    {
        return SceneObjectType::TerrainWall;
    }

    return SceneObjectType::Cube;
}

CollisionShapeType CollisionShapeFromInt(int value)
{
    if (value == 1)
    {
        return CollisionShapeType::Box;
    }

    if (value == 2)
    {
        return CollisionShapeType::Terrain;
    }

    if (value == 3)
    {
        return CollisionShapeType::Wall;
    }

    return CollisionShapeType::None;
}

}

Scene::Scene()
{
    AddCube();
}

void Scene::AddCube()
{
    SceneObject object;
    object.Name = "Cube " + std::to_string(m_Objects.size() + 1);
    object.TransformData.Position.x = static_cast<float>(m_Objects.size()) * 1.8f;
    object.Type = SceneObjectType::Cube;
    object.CollisionShape = CollisionShapeType::Box;
    object.MaterialData.Albedo = {1.0f, 1.0f, 1.0f};

    m_Objects.push_back(object);
    m_SelectedIndex = static_cast<int>(m_Objects.size()) - 1;
}

void Scene::AddTerrain()
{
    SceneObject object;
    object.Name = "Terrain " + std::to_string(m_Objects.size() + 1);
    object.Type = SceneObjectType::Terrain;
    object.CollisionShape = CollisionShapeType::Terrain;
    object.TransformData.Position = {0.0f, -0.72f, 0.0f};
    object.TransformData.Scale = {1.0f, 1.0f, 1.0f};
    object.MaterialData.Albedo = {0.42f, 0.70f, 0.34f};

    m_Objects.push_back(object);
    m_SelectedIndex = static_cast<int>(m_Objects.size()) - 1;
    ConsoleLog::Info("Added terrain with terrain collision shape");
}

void Scene::AddTerrainWall()
{
    SceneObject object;
    object.Name = "Wall " + std::to_string(m_Objects.size() + 1);
    object.Type = SceneObjectType::TerrainWall;
    object.CollisionShape = CollisionShapeType::Wall;
    object.TransformData.Position = {0.0f, 2.0f, -4.0f};
    object.TransformData.Rotation.x = 1.57079632679f;
    object.TransformData.Scale = {1.0f, 1.0f, 0.45f};
    object.MaterialData.Albedo = {0.48f, 0.62f, 0.38f};

    m_Objects.push_back(object);
    m_SelectedIndex = static_cast<int>(m_Objects.size()) - 1;
    ConsoleLog::Info("Added vertical terrain wall with wall collision shape");
}

void Scene::DeleteSelected()
{
    if (m_Objects.empty())
    {
        return;
    }

    m_Objects.erase(m_Objects.begin() + m_SelectedIndex);

    if (m_Objects.empty())
    {
        m_SelectedIndex = 0;
        return;
    }

    m_SelectedIndex = std::clamp(m_SelectedIndex, 0, static_cast<int>(m_Objects.size()) - 1);
}

void Scene::SelectNext()
{
    if (m_Objects.empty())
    {
        return;
    }

    m_SelectedIndex = (m_SelectedIndex + 1) % static_cast<int>(m_Objects.size());
}

void Scene::SelectPrevious()
{
    if (m_Objects.empty())
    {
        return;
    }

    m_SelectedIndex--;
    if (m_SelectedIndex < 0)
    {
        m_SelectedIndex = static_cast<int>(m_Objects.size()) - 1;
    }
}

void Scene::Select(int index)
{
    if (m_Objects.empty())
    {
        m_SelectedIndex = 0;
        return;
    }

    m_SelectedIndex = std::clamp(index, 0, static_cast<int>(m_Objects.size()) - 1);
}

bool Scene::SaveToFile(const std::string& path) const
{
    std::ofstream file(path);
    if (!file)
    {
        ConsoleLog::Error("Failed to save scene: " + path);
        return false;
    }

    file << "RendererScene 2\n";
    file << m_SelectedIndex << "\n";
    file << m_Objects.size() << "\n";

    for (const SceneObject& object : m_Objects)
    {
        file << object.Name << "\n";
        file << ObjectTypeToInt(object.Type) << " " << CollisionShapeToInt(object.CollisionShape) << "\n";
        file << object.TransformData.Position.x << " " << object.TransformData.Position.y << " " << object.TransformData.Position.z << "\n";
        file << object.TransformData.Rotation.x << " " << object.TransformData.Rotation.y << " " << object.TransformData.Rotation.z << "\n";
        file << object.TransformData.Scale.x << " " << object.TransformData.Scale.y << " " << object.TransformData.Scale.z << "\n";
        file << object.MaterialData.Albedo.x << " " << object.MaterialData.Albedo.y << " " << object.MaterialData.Albedo.z << "\n";
    }

    ConsoleLog::Info("Saved scene: " + path);
    return true;
}

bool Scene::LoadFromFile(const std::string& path)
{
    std::ifstream file(path);
    if (!file)
    {
        ConsoleLog::Error("Failed to load scene: " + path);
        return false;
    }

    std::string header;
    int version = 0;
    file >> header >> version;
    if (header != "RendererScene" || version < 1 || version > 2)
    {
        ConsoleLog::Error("Unsupported scene file: " + path);
        return false;
    }

    int selectedIndex = 0;
    size_t objectCount = 0;
    file >> selectedIndex;
    file >> objectCount;
    file.ignore(1024, '\n');

    std::vector<SceneObject> loadedObjects;
    for (size_t index = 0; index < objectCount; index++)
    {
        SceneObject object;
        std::getline(file, object.Name);
        if (version >= 2)
        {
            int objectType = 0;
            int collisionShape = 0;
            file >> objectType >> collisionShape;
            object.Type = ObjectTypeFromInt(objectType);
            object.CollisionShape = CollisionShapeFromInt(collisionShape);
        }
        else
        {
            object.Type = SceneObjectType::Cube;
            object.CollisionShape = CollisionShapeType::Box;
        }

        file >> object.TransformData.Position.x >> object.TransformData.Position.y >> object.TransformData.Position.z;
        file >> object.TransformData.Rotation.x >> object.TransformData.Rotation.y >> object.TransformData.Rotation.z;
        file >> object.TransformData.Scale.x >> object.TransformData.Scale.y >> object.TransformData.Scale.z;
        file >> object.MaterialData.Albedo.x >> object.MaterialData.Albedo.y >> object.MaterialData.Albedo.z;
        file.ignore(1024, '\n');

        if (object.Name.empty())
        {
            object.Name = "Unnamed";
        }

        loadedObjects.push_back(object);
    }

    m_Objects = loadedObjects;
    if (m_Objects.empty())
    {
        m_SelectedIndex = 0;
    }
    else
    {
        m_SelectedIndex = std::clamp(selectedIndex, 0, static_cast<int>(m_Objects.size()) - 1);
    }

    ConsoleLog::Info("Loaded scene: " + path);
    return true;
}

SceneObject* Scene::GetSelectedObject()
{
    if (m_Objects.empty())
    {
        return nullptr;
    }

    return &m_Objects[m_SelectedIndex];
}

const SceneObject* Scene::GetSelectedObject() const
{
    if (m_Objects.empty())
    {
        return nullptr;
    }

    return &m_Objects[m_SelectedIndex];
}

std::vector<SceneObject>& Scene::GetObjects()
{
    return m_Objects;
}

const std::vector<SceneObject>& Scene::GetObjects() const
{
    return m_Objects;
}

int Scene::GetSelectedIndex() const
{
    return m_SelectedIndex;
}

}
