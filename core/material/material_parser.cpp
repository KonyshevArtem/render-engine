#include "material_parser.h"
#include "nlohmann/json.hpp"
#include "file_system/file_system.h"
#include "shader/shader.h"
#include "texture_2d/texture_2d.h"
#include "cubemap/cubemap.h"
#include "resources/resources.h"
#include "types/graphics_backend_stencil_descriptor.h"
#include "types/graphics_backend_depth_descriptor.h"

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

NLOHMANN_JSON_SERIALIZE_ENUM(CullFace, {
    {CullFace::NONE, "NONE"},
    {CullFace::FRONT, "FRONT"},
    {CullFace::BACK, "BACK"}
})

NLOHMANN_JSON_SERIALIZE_ENUM(ComparisonFunction, {
    {ComparisonFunction::NEVER, "NEVER"},
    {ComparisonFunction::LESS, "LESS"},
    {ComparisonFunction::EQUAL, "EQUAL"},
    {ComparisonFunction::LEQUAL, "LEQUAL"},
    {ComparisonFunction::GREATER, "GREATER"},
    {ComparisonFunction::NOTEQUAL, "NOTEQUAL"},
    {ComparisonFunction::GEQUAL, "GEQUAL"},
    {ComparisonFunction::ALWAYS, "ALWAYS"}
})

NLOHMANN_JSON_SERIALIZE_ENUM(StencilOperation, {
    {StencilOperation::KEEP, "KEEP"},
    {StencilOperation::ZERO, "ZERO"},
    {StencilOperation::REPLACE, "REPLACE"},
    {StencilOperation::INCREMENT_SATURATE, "INCREMENT_SATURATE"},
    {StencilOperation::DECREMENT_SATURATE, "DECREMENT_SATURATE"},
    {StencilOperation::INVERT, "INVERT"},
    {StencilOperation::INCREMENT, "INCREMENT"},
    {StencilOperation::DECREMENT, "DECREMENT"}
})

void from_json(const nlohmann::json& json, BlendInfo& info)
{
    json.at("Enabled").get_to(info.Enabled);
    json.at("SourceFactor").get_to(info.SourceFactor);
    json.at("DestinationFactor").get_to(info.DestinationFactor);
}

void from_json(const nlohmann::json& json, CullInfo& info)
{
    json.at("Face").get_to(info.Face);
}

void from_json(const nlohmann::json& json, GraphicsBackendDepthDescriptor& depthDescriptor)
{
    json.at("WriteDepth").get_to(depthDescriptor.WriteDepth);
    if (json.contains("DepthFunction"))
        json.at("DepthFunction").get_to(depthDescriptor.DepthFunction);
}

void from_json(const nlohmann::json& json, GraphicsBackendStencilOperationDescriptor& stencilOperationDescriptor)
{
    if (json.contains("Fail"))
        json.at("Fail").get_to(stencilOperationDescriptor.FailOp);
    if (json.contains("DepthFail"))
        json.at("DepthFail").get_to(stencilOperationDescriptor.DepthFailOp);
    if (json.contains("Pass"))
        json.at("Pass").get_to(stencilOperationDescriptor.PassOp);
    if (json.contains("Comparison"))
        json.at("Comparison").get_to(stencilOperationDescriptor.ComparisonFunction);
}

void from_json(const nlohmann::json& json, GraphicsBackendStencilDescriptor& stencilDescriptor)
{
    json.at("Enabled").get_to(stencilDescriptor.Enabled);
    if (json.contains("ReadMask"))
        json.at("ReadMask").get_to(stencilDescriptor.ReadMask);
    if (json.contains("WriteMask"))
        json.at("WriteMask").get_to(stencilDescriptor.ReadMask);
    if (json.contains("FrontFace"))
        json.at("FrontFace").get_to(stencilDescriptor.FrontFaceOpDescriptor);
    if (json.contains("BackFace"))
        json.at("BackFace").get_to(stencilDescriptor.BackFaceOpDescriptor);
}

namespace MaterialParser
{
    struct ShaderInfo
    {
        std::string Path;
        std::vector<std::string> Keywords;
        BlendInfo BlendInfo;
        CullInfo CullInfo;
        GraphicsBackendDepthDescriptor DepthDescriptor;
        GraphicsBackendStencilDescriptor StencilDescriptor;
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
        if (json.contains("CullInfo"))
            json.at("CullInfo").get_to(info.CullInfo);
        if (json.contains("DepthInfo"))
            json.at("DepthInfo").get_to(info.DepthDescriptor);
        if (json.contains("Stencil"))
            json.at("Stencil").get_to(info.StencilDescriptor);
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

    template<typename T>
    void LoadTexture(std::shared_ptr<Material> material, const TextureInfo& textureInfo, bool async)
    {
        if (async)
        {
            material->SetTexture(textureInfo.Name, Texture2D::White());

            const std::string& name = textureInfo.Name;
            Resources::LoadAsync<T>(textureInfo.Path, [material, name](std::shared_ptr<T> texture)
                                            { material->SetTexture(name, texture); });
        }
        else
            material->SetTexture(textureInfo.Name, Resources::Load<T>(textureInfo.Path));
    }

    std::shared_ptr<Material> Parse(const std::filesystem::path& path, bool asyncTextureLoads)
    {
        std::string materialText = FileSystem::ReadFile(FileSystem::GetResourcesPath() / path);
        nlohmann::json materialJson = nlohmann::json::parse(materialText);

        MaterialInfo materialInfo;
        materialJson.get_to(materialInfo);

        std::shared_ptr<Shader> shader = Shader::Load(materialInfo.Shader.Path, materialInfo.Shader.Keywords, materialInfo.Shader.BlendInfo,
                                                          materialInfo.Shader.CullInfo, materialInfo.Shader.DepthDescriptor);
        std::shared_ptr<Material> material = std::make_shared<Material>(shader, path.string());

        material->StencilDescriptor = materialInfo.Shader.StencilDescriptor;

        for (const TextureInfo& textureInfo: materialInfo.Textures)
        {
            bool isWhite = textureInfo.Path == "White";
            bool isNormal = textureInfo.Path == "Normal";

            if (textureInfo.Type == "2D")
            {
                if (isWhite)
                    material->SetTexture(textureInfo.Name, Texture2D::White());
                else if (isNormal)
                    material->SetTexture(textureInfo.Name, Texture2D::Normal());
                else
                    LoadTexture<Texture2D>(material, textureInfo, asyncTextureLoads);
            }
            else if (textureInfo.Type == "Cube")
            {
                if (isWhite)
                    material->SetTexture(textureInfo.Name, Cubemap::White());
                else
                    LoadTexture<Cubemap>(material, textureInfo, asyncTextureLoads);
            }
        }

        for (const FloatInfo& floatInfo : materialInfo.Floats)
            material->SetFloat(floatInfo.Name, floatInfo.Value);

        material->SetRenderQueue(materialInfo.RenderQueue);

        return material;
    }
}