#ifndef RENDER_ENGINE_PBR_DEMO_UPDATER_H
#define RENDER_ENGINE_PBR_DEMO_UPDATER_H

#include "component/component.h"

class PBRDemoUpdater : public Component
{
public:
    static std::shared_ptr<PBRDemoUpdater> Create(const nlohmann::json& componentData);

    PBRDemoUpdater() = default;
    ~PBRDemoUpdater() = default;

    PBRDemoUpdater(const PBRDemoUpdater &) = delete;
    PBRDemoUpdater(PBRDemoUpdater &&) = delete;

    PBRDemoUpdater &operator=(const PBRDemoUpdater &) = delete;
    PBRDemoUpdater &operator=(PBRDemoUpdater &&) = delete;

    void Update() override;

private:
    bool m_Initialized = false;
    std::weak_ptr<GameObject> m_DirLight;
};

#endif //RENDER_ENGINE_PBR_DEMO_UPDATER_H
