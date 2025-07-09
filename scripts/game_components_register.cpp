#include "game_components_register.h"
#include "component/component.h"
#include "camera_fly_controller/camera_fly_controller.h"
#include "pbr_demo/pbr_demo_updater.h"
#include "test_scene/flashlight.h"
#include "test_scene/test_scene_updater.h"

REGISTER_COMPONENT(CameraFlyController)
REGISTER_COMPONENT(PBRDemoUpdater)
REGISTER_COMPONENT(Flashlight)
REGISTER_COMPONENT(TestSceneUpdater)

void GameComponents::Register()
{
    // Stub
}