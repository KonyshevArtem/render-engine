#ifndef RENDER_ENGINE_SHADER_COMPILER_SERIALIZATION_H
#define RENDER_ENGINE_SHADER_COMPILER_SERIALIZATION_H

#include "reflection_common.h"
#include "nlohmann/json.hpp"

#include <filesystem>

void to_json(nlohmann::json& json, const BufferDescriptor& bufferDesc)
{
	json = nlohmann::json{
			{"Size",      nlohmann::json(bufferDesc.Size)},
			{"BufferType", nlohmann::json(bufferDesc.BufferType)},
			{"ReadWrite", nlohmann::json(bufferDesc.ReadWrite)},
			{"Binding",  nlohmann::json(bufferDesc.Binding)},
			{"Variables", nlohmann::json(bufferDesc.Variables)},
	};
}

void to_json(nlohmann::json& json, const TextureDescriptor& desc)
{
	json = nlohmann::json{
			{"Binding",  nlohmann::json(desc.Binding)},
			  {"ReadWrite", nlohmann::json(desc.ReadWrite)}
	};
}

void to_json(nlohmann::json& json, const ResourceDescriptor& desc)
{
    json = nlohmann::json{
            {"Binding",  nlohmann::json(desc.Binding)},
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

inline void WriteReflection(const std::filesystem::path& outputDirPath, const Reflection& reflection)
{
    const std::string json = nlohmann::json(reflection).dump();

    const std::filesystem::path outputPath = std::filesystem::absolute(outputDirPath / "reflection.json");
    std::filesystem::create_directories(outputPath.parent_path());

    FILE* fp;
	fopen_s(&fp, outputPath.string().c_str(), "w");
    fwrite(json.c_str(), json.size(), 1, fp);
    fclose(fp);
}

#endif //RENDER_ENGINE_SHADER_COMPILER_SERIALIZATION_H
