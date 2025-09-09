#include "flashlight.h"
#include "gameObject/gameObject.h"
#include "camera/camera.h"

std::shared_ptr<Flashlight> Flashlight::Create(const nlohmann::json& componentData)
{
    return std::make_shared<Flashlight>();
}

DEFINE_COMPONENT_DEFAULT_ASYNC_CONSTRUCTOR(Flashlight)

void Flashlight::Update()
{
    GetGameObject()->SetPosition(Camera::Current->GetGameObject()->GetPosition() + Camera::Current->GetGameObject()->GetRotation() * Vector3(-3, 0, 0));
    GetGameObject()->SetRotation(Camera::Current->GetGameObject()->GetRotation());
}
