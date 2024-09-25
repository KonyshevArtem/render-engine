#ifndef RENDER_ENGINE_SHADER_COMPILER_SERIALIZATION_H
#define RENDER_ENGINE_SHADER_COMPILER_SERIALIZATION_H

#include "reflection.h"

#include <boost/json/value_from.hpp>
#include <boost/json/serialize.hpp>

void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, const Bindings &bindings)
{
    jv = {
            {"Vertex",   bindings.Vertex},
            {"Fragment", bindings.Fragment}
    };
}

void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, const BufferDesc &bufferDesc)
{
    jv = {
            {"Size",      bufferDesc.Size},
            {"Bindings",  boost::json::value_from(bufferDesc.Bindings)},
            {"Variables", boost::json::value_from(bufferDesc.Variables)},
    };
}

void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, const TextureDesc &textureDesc)
{
    jv = {
            {"Bindings",  boost::json::value_from(textureDesc.Bindings)},
            {"HasSampler", textureDesc.HasSampler},
    };
}

void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, const Reflection &reflection)
{
    jv = {
            {"Buffers",  boost::json::value_from(reflection.Buffers)},
            {"Textures", boost::json::value_from(reflection.Textures)}
    };
}

void WriteReflection(const std::filesystem::path &hlslPath, const Reflection &reflection)
{
    std::string json = boost::json::serialize(boost::json::value_from(reflection));

    std::filesystem::path outputPath = hlslPath.parent_path() / "output" / "reflection.json";
    std::filesystem::create_directory(outputPath.parent_path());

    FILE *fp = fopen(outputPath.c_str(), "w");
    fwrite(json.c_str(), json.size(), 1, fp);
    fclose(fp);
}

#endif //RENDER_ENGINE_SHADER_COMPILER_SERIALIZATION_H
