#include "material_parser.h"
#include "nlohmann/json.hpp"
#include "file_system/file_system.h"
#include "shader/shader.h"
#include "texture_2d/texture_2d.h"
#include "cubemap/cubemap.h"
#include "resources/resources.h"

NLOHMANN_JSON_SERIALIZE_ENUM(BlendFactor, {
    {BlendFactor::ZERO, "ZERO"},
    {BlendFactor::ONE, "ONE"},
    {BlendFactor::SRC_COLOR, "SRC_COLOR"},
    {BlendFactor::ONE_MINUS_SRC_COLOR, "ONE_MINUS_SRC_COLOR"},
    {BlendFactor::DST_COLOR, "DST_COLOR"},
    {BlendFactor::ONE_MINUS_DST_COLOR, "ONE_MINUS_DST_COLOR"},
    {BlendFactor::SRC_ALPHA, "SRC_ALPHA"},
    {BlendFactor::ONE_MINUS_SRC_ALPHA, "ONE_MINUS_SRC_ALPHA"},
    {BlendFactor::DST_ALPHA, "DST_ALPHA"},
    {BlendFactor::ONE_MINUS_DST_ALPHA, "ONE_MINUS_DST_ALPHA"},
    {BlendFactor::CONSTANT_COLOR, "CONSTANT_COLOR"},
    {BlendFactor::ONE_MINUS_CONSTANT_COLOR, "ONE_MINUS_CONSTANT_COLOR"},
    {BlendFactor::CONSTANT_ALPHA, "CONSTANT_ALPHA"},
    {BlendFactor::ONE_MINUS_CONSTANT_ALPHA, "ONE_MINUS_CONSTANT_ALPHA"},
    {BlendFactor::SRC_ALPHA_SATURATE, "SRC_ALPHA_SATURATE"},
    {BlendFactor::SRC1_COLOR, "SRC1_COLOR"},
    {BlendFactor::ONE_MINUS_SRC1_COLOR, "ONE_MINUS_SRC1_COLOR"},
    {BlendFactor::SRC1_ALPHA, "SRC1_ALPHA"},
    {BlendFactor::ONE_MINUS_SRC1_ALPHA, "ONE_MINUS_SRC1_ALPHA"},
})

void from_json(const nlohmann::json& json, BlendInfo& info)
{
    json.at("Enabled").get_to(info.Enabled);
    json.at("SourceFactor").get_to(info.SourceFactor);
    json.at("DestinationFactor").get_to(info.DestinationFactor);
}

namespace MaterialParser
{
    struct ShaderInfo
    {
        std::string Path;
        std::vector<std::string> Keywords;
        BlendInfo BlendInfo;
    };

    struct TextureInfo
    {
        std::string Name;
        std::string Type;
        std::string Path;
    };

    struct FloatInfo
    {
        std::string Name;
        float Value = 0;
    };

    struct MaterialInfo
    {
        ShaderInfo Shader;
        std::vector<TextureInfo> Textures;
        std::vector<FloatInfo> Floats;
        float RenderQueue = 2000;
    };

    void from_json(const nlohmann::json& json, ShaderInfo& info)
    {
        json.at("Path").get_to(info.Path);
        if (json.contains("Keywords"))
            json.at("Keywords").get_to(info.Keywords);
        if (json.contains("BlendInfo"))
            json.at("BlendInfo").get_to(info.BlendInfo);
    }

    void from_json(const nlohmann::json& json, TextureInfo& info)
    {
        json.at("Name").get_to(info.Name);
        json.at("Type").get_to(info.Type);
        json.at("Path").get_to(info.Path);
    }

    void from_json(const nlohmann::json& json, FloatInfo& info)
    {
        json.at("Name").get_to(info.Name);
        json.at("Value").get_to(info.Value);
    }

    void from_json(const nlohmann::json& json, MaterialInfo& info)
    {
        json.at("Shader").get_to(info.Shader);
        if (json.contains("Textures"))
            json.at("Textures").get_to(info.Textures);
        if (json.contains("Floats"))
            json.at("Floats").get_to(info.Floats);
        if (json.contains("RenderQueue"))
            json.at("RenderQueue").get_to(info.RenderQueue);
    }

    std::shared_ptr<Material> Parse(const std::filesystem::path& path)
    {
        std::string materialText = FileSystem::ReadFile(FileSystem::GetResourcesPath() / path);
        nlohmann::json materialJson = nlohmann::json::parse(materialText);

        MaterialInfo materialInfo;
        materialJson.get_to(materialInfo);

        std::shared_ptr<Shader> shader = Shader::Load(materialInfo.Shader.Path, materialInfo.Shader.Keywords, materialInfo.Shader.BlendInfo, {}, {});
        std::shared_ptr<Material> material = std::make_shared<Material>(shader, path.string());

        for (const TextureInfo& textureInfo: materialInfo.Textures)
        {
            bool isWhite = textureInfo.Path == "White";
            bool isNormal = textureInfo.Path == "Normal";

            std::shared_ptr<Texture> texture;
            if (textureInfo.Type == "2D")
            {
                if (isWhite)
                    texture = Texture2D::White();
                else if (isNormal)
                    texture = Texture2D::Normal();
                else
                    texture = Resources::Load<Texture2D>(textureInfo.Path);
            }
            else if (textureInfo.Type == "Cube")
            {
                if (isWhite)
                    texture = Cubemap::White();
                else
                    texture = Resources::Load<Cubemap>(textureInfo.Path);
            }

            material->SetTexture(textureInfo.Name, texture);
        }

        for (const FloatInfo& floatInfo : materialInfo.Floats)
            material->SetFloat(floatInfo.Name, floatInfo.Value);

        material->SetRenderQueue(materialInfo.RenderQueue);

        return material;
    }
}