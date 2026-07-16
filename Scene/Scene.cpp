#include "Scene/Scene.h"

#include <algorithm>

namespace Engine
{

Scene::Scene()
{
    AddCube();
}

void Scene::AddCube()
{
    SceneObject object;
    object.Name = "Cube " + std::to_string(m_Objects.size() + 1);
    object.TransformData.Position.x = static_cast<float>(m_Objects.size()) * 1.8f;

    m_Objects.push_back(object);
    m_SelectedIndex = static_cast<int>(m_Objects.size()) - 1;
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
