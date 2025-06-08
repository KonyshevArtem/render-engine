#ifndef RENDER_ENGINE_SHADER_COMPILER_SERIALIZATION_H
#define RENDER_ENGINE_SHADER_COMPILER_SERIALIZATION_H

#include "reflection.h"
#include "graphics_backend.h"
#include "nlohmann/json.hpp"

#include <filesystem>

void to_json(nlohmann::json& json, const Bindings& bindings)
{
    json = nlohmann::json{
            {"Vertex",   bindings.Vertex},
            {"Fragment", bindings.Fragment}
    };
}

void to_json(nlohmann::json& json, const BufferDesc& bufferDesc)
{
    json = nlohmann::json{
            {"Size",      bufferDesc.Size},
            {"IsConstant", bufferDesc.IsConstant},
            {"Bindings",  nlohmann::json(bufferDesc.Bindings)},
            {"Variables", nlohmann::json(bufferDesc.Variables)},
    };
}

void to_json(nlohmann::json& json, const GenericDesc& desc)
{
    json = nlohmann::json{
            {"Bindings",  nlohmann::json(desc.Bindings)},
    };
}

void to_json(nlohmann::json& json, const Reflection& reflection)
{
    std::unordered_map<std::string, int> a;

    json = nlohmann::json{
            {"Buffers",  nlohmann::json(reflection.Buffers)},
            {"Textures", nlohmann::json(reflection.Textures)},
            {"Samplers", nlohmann::json(reflection.Samplers)},
    };
}

void WriteReflection(const std::filesystem::path &outputDirPath, const Reflection &reflection)
{
    std::string json = nlohmann::json(reflection).dump();

    std::filesystem::path outputPath = outputDirPath / "reflection.json";
    std::filesystem::create_directory(outputPath.parent_path());

    FILE *fp = fopen(outputPath.string().c_str(), "w");
    fwrite(json.c_str(), json.size(), 1, fp);
    fclose(fp);
}

#endif //RENDER_ENGINE_SHADER_COMPILER_SERIALIZATION_H
