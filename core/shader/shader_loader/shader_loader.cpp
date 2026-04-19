#include "shader_loader.h"
#include "shader/shader.h"
#include "debug.h"
#include "graphics/graphics.h"
#include "file_system/file_system.h"
#include "enums/shader_type.h"
#include "types/graphics_backend_shader_object.h"
#include "graphics_backend_api.h"
#include "shader_parser.h"
#include "hash.h"

#include <span>

namespace ShaderLoader
{
    const std::string INSTANCING_KEYWORD = "_INSTANCING";

    std::string GetBackendLiteral(GraphicsBackendName backendName)
    {
        switch (backendName)
        {
            case GraphicsBackendName::OPENGL:
                return "opengl";
            case GraphicsBackendName::GLES:
                return "gles";
            case GraphicsBackendName::METAL:
                return "metal";
            case GraphicsBackendName::DX12:
                return "dx12";
            default:
                return "";
        }
    }

    std::string GetKeywordsHash(std::vector<std::string> keywords, bool& outSupportInstancing)
    {
        outSupportInstancing = false;

        std::string keywordsDirectives;
        std::sort(keywords.begin(), keywords.end());
        for (const auto &keyword: keywords)
        {
            keywordsDirectives += keyword + ",";
            outSupportInstancing |= keyword == INSTANCING_KEYWORD;
        }

        return std::to_string(Hash::FNV1a(keywordsDirectives));
    }

	std::shared_ptr<Shader> Load(const std::filesystem::path& path, const std::vector<std::string>& keywords)
    {
        bool supportInstancing = false;
        std::string keywordHash = GetKeywordsHash(keywords, supportInstancing);

        try
        {
            std::string backendLiteral = GetBackendLiteral(GraphicsBackend::Current()->GetName());
            std::filesystem::path backendPath = FileSystem::GetResourcesPath() / path / backendLiteral / keywordHash;

            auto reflectionJson = FileSystem::ReadFile(backendPath / "reflection.json");
            std::unordered_map<std::string, GraphicsBackendTextureInfo> textures;
            std::unordered_map<std::string, GraphicsBackendSamplerInfo> samplers;
            std::unordered_map<std::string, std::shared_ptr<GraphicsBackendBufferInfo>> buffers;
            std::unordered_map<std::string, GraphicsBackendTLASInfo> TLASes;
            ThreadGroupSize threadGroupSize;
            ShaderParser::ParseReflection(reflectionJson, textures, buffers, samplers, TLASes, threadGroupSize);

            std::string shaderDebugName;
            shaderDebugName.append(path.string());
            shaderDebugName.append("_");
            shaderDebugName.append(keywordHash);

            std::vector<GraphicsBackendShaderObject> shaders;
            for (int i = 0; i < static_cast<int>(ShaderType::COUNT); ++i)
            {
                const ShaderType shaderType = static_cast<ShaderType>(i);
                const std::string& shaderFilename = GraphicsBackendBase::GetShaderTypeName(shaderType);

                std::filesystem::path sourcePath = backendPath / shaderFilename;
                if (!FileSystem::FileExists(sourcePath))
                    continue;

                std::string shaderFunctionDebugName = shaderDebugName;
                shaderFunctionDebugName.append("_");
                shaderFunctionDebugName.append(shaderFilename);

                GraphicsBackendShaderObject shader{};
                if (GraphicsBackend::Current()->GetName() == GraphicsBackendName::DX12)
                {
                    std::vector<uint8_t> shaderBinary;
                    FileSystem::ReadFileBytes(sourcePath, shaderBinary);
                    shader = GraphicsBackend::Current()->CompileShaderBinary(shaderType, shaderBinary, shaderFunctionDebugName);
                }
                else
                {
                    std::string shaderSource = FileSystem::ReadFile(sourcePath);
                    shader = GraphicsBackend::Current()->CompileShader(shaderType, shaderSource, shaderFunctionDebugName);
                }
                shaders.push_back(shader);
            }

            return std::make_shared<Shader>(shaders, textures, buffers, samplers, TLASes, threadGroupSize, shaderDebugName, supportInstancing);
        }
        catch (const std::exception &_exception)
        {
            std::string keywordString = keywords.empty() ? "<no defines>" : "";
            for (const std::string& keyword : keywords)
                keywordString += keyword + " ";

            Debug::LogErrorFormat("[ShaderLoader] Can't load shader {}\n{}\n{}", path.string(), keywordString, _exception.what());
            return nullptr;
        }
    }
} // namespace ShaderLoader