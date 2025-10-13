#include "shader_parser.h"
#include "nlohmann/json.hpp"

void from_json(const nlohmann::json& json, GraphicsBackendTextureInfo& info)
{
    json.at("Binding").get_to(info.Binding);
}

void from_json(const nlohmann::json& json, GraphicsBackendSamplerInfo& info)
{
    json.at("Binding").get_to(info.Binding);
}

template <>
struct nlohmann::adl_serializer<std::shared_ptr<GraphicsBackendBufferInfo>>
{
    static std::shared_ptr<GraphicsBackendBufferInfo> from_json(const nlohmann::json& json)
    {
        uint32_t binding;
        uint32_t size;
        BufferType bufferType;
        std::unordered_map<std::string, int> variables;

        json.at("Binding").get_to(binding);
        json.at("Size").get_to(size);
        json.at("BufferType").get_to(bufferType);
        json.at("Variables").get_to(variables);

        auto info = std::make_shared<GraphicsBackendBufferInfo>(binding, size, bufferType, variables);
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
