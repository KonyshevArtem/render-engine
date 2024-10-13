#include "shader_parser.h"
#include <boost/json.hpp>

GraphicsBackendResourceBindings tag_invoke(boost::json::value_to_tag<GraphicsBackendResourceBindings>, const boost::json::value& jv)
{
    GraphicsBackendResourceBindings bindings;
    bindings.VertexIndex = boost::json::value_to<int>(jv.at("Vertex"));
    bindings.FragmentIndex = boost::json::value_to<int>(jv.at("Fragment"));
    return bindings;
}

GraphicsBackendTextureInfo tag_invoke(boost::json::value_to_tag<GraphicsBackendTextureInfo>, const boost::json::value& jv)
{
    GraphicsBackendTextureInfo info{};
    info.TextureBindings = boost::json::value_to<GraphicsBackendResourceBindings>(jv.at("Bindings"));
    return info;
}

GraphicsBackendSamplerInfo tag_invoke(boost::json::value_to_tag<GraphicsBackendSamplerInfo>, const boost::json::value& jv)
{
    GraphicsBackendSamplerInfo info{};
    info.Bindings = boost::json::value_to<GraphicsBackendResourceBindings>(jv.at("Bindings"));
    return info;
}

std::shared_ptr<GraphicsBackendBufferInfo> tag_invoke(boost::json::value_to_tag<std::shared_ptr<GraphicsBackendBufferInfo>>, const boost::json::value& jv)
{
    auto bindings = boost::json::value_to<GraphicsBackendResourceBindings>(jv.at("Bindings"));
    int size = boost::json::value_to<int>(jv.at("Size"));
    auto variables = boost::json::value_to<std::unordered_map<std::string, int>>(jv.at("Variables"));

    auto info = std::make_shared<GraphicsBackendBufferInfo>(GraphicsBackendBufferInfo::BufferType::UNIFORM, size, variables);
    info->SetBindings(bindings);
    return info;
}

namespace ShaderParser
{
    void ParseReflection(const std::string& reflectionJson,
        std::unordered_map<std::string, GraphicsBackendTextureInfo>& textures,
        std::unordered_map<std::string, std::shared_ptr<GraphicsBackendBufferInfo>>& buffers,
        std::unordered_map<std::string, GraphicsBackendSamplerInfo>& samplers)
    {
        auto reflectionObject = boost::json::parse(reflectionJson).as_object();
        buffers = std::move(boost::json::value_to<std::unordered_map<std::string, std::shared_ptr<GraphicsBackendBufferInfo>>>(reflectionObject["Buffers"]));
        textures = std::move(boost::json::value_to<std::unordered_map<std::string, GraphicsBackendTextureInfo>>(reflectionObject["Textures"]));
        samplers = std::move(boost::json::value_to<std::unordered_map<std::string, GraphicsBackendSamplerInfo>>(reflectionObject["Samplers"]));
    }
}
