#include "json_common.h"

void from_json(const nlohmann::json& json, Vector3& vector)
{
    json.at("x").get_to(vector.x);
    json.at("y").get_to(vector.y);
    json.at("z").get_to(vector.z);
}

void from_json(const nlohmann::json& json, Quaternion& vector)
{
    json.at("x").get_to(vector.x);
    json.at("y").get_to(vector.y);
    json.at("z").get_to(vector.z);
    json.at("w").get_to(vector.w);
}