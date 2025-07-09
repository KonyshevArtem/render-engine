#ifndef RENDER_ENGINE_JSON_COMMON_H
#define RENDER_ENGINE_JSON_COMMON_H

#include "nlohmann/json.hpp"
#include "vector3/vector3.h"
#include "quaternion/quaternion.h"

void from_json(const nlohmann::json& json, Vector3& vector);
void from_json(const nlohmann::json& json, Quaternion& vector);

#endif //RENDER_ENGINE_JSON_COMMON_H
