#ifndef RENDER_ENGINE_FLASHLIGHT_H
#define RENDER_ENGINE_FLASHLIGHT_H

#include "component/component.h"

class Flashlight : public Component
{
public:
    DECLARE_COMPONENT_CONSTRUCTORS(Flashlight)

    Flashlight() = default;
    ~Flashlight() = default;

    Flashlight(const Flashlight &) = delete;
    Flashlight(Flashlight &&) = delete;

    Flashlight &operator=(const Flashlight &) = delete;
    Flashlight &operator=(Flashlight &&) = delete;

    void Update() override;
};

#endif //RENDER_ENGINE_FLASHLIGHT_H
