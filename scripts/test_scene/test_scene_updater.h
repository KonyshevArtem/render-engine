#ifndef RENDER_ENGINE_TEST_SCENE_UPDATER_H
#define RENDER_ENGINE_TEST_SCENE_UPDATER_H

#include "component/component.h"

class Material;

class TestSceneUpdater : public Component
{
public:
    static std::shared_ptr<TestSceneUpdater> Create(const nlohmann::json& componentData);

    TestSceneUpdater() = default;
    ~TestSceneUpdater() = default;

    TestSceneUpdater(const TestSceneUpdater &) = delete;
    TestSceneUpdater(TestSceneUpdater &&) = delete;

    TestSceneUpdater &operator=(const TestSceneUpdater &) = delete;
    TestSceneUpdater &operator=(TestSceneUpdater &&) = delete;

    void Update() override;

private:
    bool m_Initialized = false;
    std::weak_ptr<GameObject> m_DirLight;
    std::weak_ptr<GameObject> m_RotatingCube;
    std::weak_ptr<GameObject> m_RotatingCylinder1;
    std::weak_ptr<GameObject> m_RotatingCylinder2;
    std::weak_ptr<Material> m_WaterMaterial;
};

#endif //RENDER_ENGINE_TEST_SCENE_UPDATER_H
