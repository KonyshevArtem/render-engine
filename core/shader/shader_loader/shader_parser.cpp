#include "shader_parser.h"
#include "nlohmann/json.hpp"

void from_json(const nlohmann::json& json, GraphicsBackendTextureInfo& info)
{
    json.at("Binding").get_to(info.Binding);
    json.at("ReadWrite").get_to(info.ReadWrite);
}

void from_json(const nlohmann::json& json, GraphicsBackendSamplerInfo& info)
{
    json.at("Binding").get_to(info.Binding);
}

void from_json(const nlohmann::json& json, GraphicsBackendTLASInfo& info)
{
    json.at("Binding").get_to(info.Binding);
}

void from_json(const nlohmann::json& json, ThreadGroupSize& threadGroupSize)
{
    json.at("X").get_to(threadGroupSize.X);
    json.at("Y").get_to(threadGroupSize.Y);
    json.at("Z").get_to(threadGroupSize.Z);
}

template <>
struct nlohmann::adl_serializer<std::shared_ptr<GraphicsBackendBufferInfo>>
{
    static std::shared_ptr<GraphicsBackendBufferInfo> from_json(const nlohmann::json& json)
    {
        uint32_t binding;
        uint32_t size;
        BufferType bufferType;
        bool readWrite;
        std::unordered_map<std::string, int> variables;

        json.at("Binding").get_to(binding);
        json.at("Size").get_to(size);
        json.at("BufferType").get_to(bufferType);
        json.at("ReadWrite").get_to(readWrite);
        json.at("Variables").get_to(variables);

        auto info = std::make_shared<GraphicsBackendBufferInfo>(binding, size, bufferType, readWrite, variables);
        return info;
    }
};

namespace ShaderParser
{
    void ParseReflection(const std::string& reflectionJson,
        std::unordered_map<std::string, GraphicsBackendTextureInfo>& textures,
        std::unordered_map<std::string, std::shared_ptr<GraphicsBackendBufferInfo>>& buffers,
        std::unordered_map<std::string, GraphicsBackendSamplerInfo>& samplers,
        std::unordered_map<std::string, GraphicsBackendTLASInfo>& TLASes,
        ThreadGroupSize& threadGroupSize)
    {
        nlohmann::json reflectionObject = nlohmann::json::parse(reflectionJson);
        buffers = reflectionObject.at("Buffers").template get<std::unordered_map<std::string, std::shared_ptr<GraphicsBackendBufferInfo>>>();
        textures = reflectionObject.at("Textures").template get<std::unordered_map<std::string, GraphicsBackendTextureInfo>>();
        samplers = reflectionObject.at("Samplers").template get<std::unordered_map<std::string, GraphicsBackendSamplerInfo>>();
        TLASes = reflectionObject.at("TLASes").template get<std::unordered_map<std::string, GraphicsBackendTLASInfo>>();
        reflectionObject.at("ThreadGroupSize").get_to(threadGroupSize);
    }
}
