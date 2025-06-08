#include "shader_parser.h"
#include "nlohmann/json.hpp"

void from_json(const nlohmann::json& json, GraphicsBackendResourceBindings& bindings)
{
    json.at("Vertex").get_to(bindings.VertexIndex);
    json.at("Fragment").get_to(bindings.FragmentIndex);
    json.at("Space").get_to(bindings.Space);
}

void from_json(const nlohmann::json& json, GraphicsBackendTextureInfo& info)
{
    json.at("Bindings").get_to(info.TextureBindings);
}

void from_json(const nlohmann::json& json, GraphicsBackendSamplerInfo& info)
{
    json.at("Bindings").get_to(info.Bindings);
}

template <>
struct nlohmann::adl_serializer<std::shared_ptr<GraphicsBackendBufferInfo>>
{
    static std::shared_ptr<GraphicsBackendBufferInfo> from_json(const nlohmann::json& json)
    {
        int size;
        bool isConstant;
        GraphicsBackendResourceBindings bindings;
        std::unordered_map<std::string, int> variables;

        json.at("Bindings").get_to(bindings);
        json.at("Size").get_to(size);
        json.at("IsConstant").get_to(isConstant);
        json.at("Variables").get_to(variables);

        auto info = std::make_shared<GraphicsBackendBufferInfo>(size, isConstant, variables);
        info->SetBindings(bindings);
        return info;
    }
};

namespace ShaderParser
{
    void ParseReflection(const std::string& reflectionJson,
        std::unordered_map<std::string, GraphicsBackendTextureInfo>& textures,
        std::unordered_map<std::string, std::shared_ptr<GraphicsBackendBufferInfo>>& buffers,
        std::unordered_map<std::string, GraphicsBackendSamplerInfo>& samplers)
    {
        nlohmann::json reflectionObject = nlohmann::json::parse(reflectionJson);
        buffers = std::move(reflectionObject.at("Buffers").template get<std::unordered_map<std::string, std::shared_ptr<GraphicsBackendBufferInfo>>>());
        textures = std::move(reflectionObject.at("Textures").template get<std::unordered_map<std::string, GraphicsBackendTextureInfo>>());
        samplers = std::move(reflectionObject.at("Samplers").template get<std::unordered_map<std::string, GraphicsBackendSamplerInfo>>());
    }
}
