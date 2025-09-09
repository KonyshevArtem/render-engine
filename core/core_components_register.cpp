#include "core_components_register.h"
#include "component/component.h"
#include "renderer/mesh_renderer.h"
#include "renderer/billboard_renderer.h"
#include "light/light.h"
#include "camera/camera.h"

REGISTER_COMPONENT(Camera)
REGISTER_COMPONENT(MeshRenderer)
REGISTER_COMPONENT(BillboardRenderer)
REGISTER_COMPONENT(Light)

void CoreComponents::Register()
{
    // Stub
}