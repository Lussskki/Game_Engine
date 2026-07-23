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
    if (type == SceneObjectType::Circle)
    {
        return 1;
    }

    if (type == SceneObjectType::Terrain)
    {
        return 2;
    }

    if (type == SceneObjectType::Character)
    {
        return 4;
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

    return 0;
}
SceneObjectType ObjectTypeFromInt(int value)
{
    if (value == 1)
    {
        return SceneObjectType::Circle;
    }

    if (value == 2 || value == 3)
    {
        return SceneObjectType::Terrain;
    }

    if (value == 4)
    {
        return SceneObjectType::Character;
    }

    return SceneObjectType::Cube;
}

bool HasPrefix(const std::string& text, const std::string& prefix)
{
    return text.size() >= prefix.size() && text.compare(0, prefix.size(), prefix) == 0;
}
CollisionShapeType CollisionShapeFromInt(int value)
{
    if (value == 1)
    {
        return CollisionShapeType::Box;
    }

    if (value == 2 || value == 3)
    {
        return CollisionShapeType::Terrain;
    }

    return CollisionShapeType::None;
}

}

Scene::Scene()
{
}

void Scene::NewProject(const std::string& projectName, const std::string& sceneName)
{
    SetProjectName(projectName);
    SetSceneName(sceneName);
    m_Objects.clear();
    m_SelectedIndex = 0;
}

void Scene::SetProjectName(const std::string& projectName)
{
    m_ProjectName = projectName.empty() ? "Untitled Project" : projectName;
}

void Scene::SetSceneName(const std::string& sceneName)
{
    m_SceneName = sceneName.empty() ? "Main Scene" : sceneName;
}

const std::string& Scene::GetProjectName() const
{
    return m_ProjectName;
}

const std::string& Scene::GetSceneName() const
{
    return m_SceneName;
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

void Scene::AddCircle()
{
    SceneObject object;
    object.Name = "Circle " + std::to_string(m_Objects.size() + 1);
    object.TransformData.Position.x = static_cast<float>(m_Objects.size()) * 1.8f;
    object.Type = SceneObjectType::Circle;
    object.CollisionShape = CollisionShapeType::Box;
    object.MaterialData.Albedo = {0.75f, 0.85f, 1.0f};

    m_Objects.push_back(object);
    m_SelectedIndex = static_cast<int>(m_Objects.size()) - 1;
    ConsoleLog::Info("Added circle model");
}

void Scene::AddCharacter()
{
    SceneObject object;
    object.Name = "Character " + std::to_string(m_Objects.size() + 1);
    object.Type = SceneObjectType::Character;
    object.CollisionShape = CollisionShapeType::Box;
    object.TransformData.Position = {0.0f, -0.35f, 0.0f};
    object.TransformData.Scale = {0.45f, 0.45f, 0.45f};
    object.MaterialData.Albedo = {0.82f, 0.82f, 0.90f};

    m_Objects.push_back(object);
    m_SelectedIndex = static_cast<int>(m_Objects.size()) - 1;
    ConsoleLog::Info("Added playable character with box collision shape");
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

    file << "RendererScene 4\n";
    file << m_ProjectName << "\n";
    file << m_SceneName << "\n";
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
    if (header != "RendererScene" || version < 1 || version > 4)
    {
        ConsoleLog::Error("Unsupported scene file: " + path);
        return false;
    }

    std::string projectName = "Untitled Project";
    std::string sceneName = "Main Scene";
    if (version >= 3)
    {
        file.ignore(1024, '\n');
        std::getline(file, projectName);
        if (projectName.empty())
        {
            projectName = "Untitled Project";
        }

        if (version >= 4)
        {
            std::getline(file, sceneName);
            if (sceneName.empty())
            {
                sceneName = "Main Scene";
            }
        }
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
            if (objectType == 0 && HasPrefix(object.Name, "Character"))
            {
                object.Type = SceneObjectType::Character;
            }
            else if (objectType == 0 && HasPrefix(object.Name, "Circle"))
            {
                object.Type = SceneObjectType::Circle;
            }
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

    m_ProjectName = projectName;
    m_SceneName = sceneName;
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






