#pragma once

#include "Scene/Transform.h"

#include <string>
#include <vector>

namespace Engine
{

struct SceneObject
{
    std::string Name;
    Transform TransformData;
};

class Scene
{
public:
    Scene();

    void AddCube();
    void DeleteSelected();
    void SelectNext();
    void SelectPrevious();
    void Select(int index);

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
