#ifndef RENDER_ENGINE_LIGHT_H
#define RENDER_ENGINE_LIGHT_H

#include "component/component.h"
#include "quaternion/quaternion.h"
#include "vector3/vector3.h"

enum class LightType
{
    DIRECTIONAL,
    POINT,
    SPOT
};

class Light : public Component
{
public:
    DECLARE_COMPONENT_CONSTRUCTORS(Light)

    Light();
    ~Light();

    Light(const Light &) = delete;
    Light(Light &&)      = delete;

    Light &operator=(const Light &) = delete;
    Light &operator=(Light &&) = delete;

    LightType  Type;
    Vector3    Intensity;
    float      Range;
    float      CutOffAngle;

private:
    static std::vector<Light*> s_Lights;
    int m_Index;

    friend class Context;
};

#endif //RENDER_ENGINE_LIGHT_H
