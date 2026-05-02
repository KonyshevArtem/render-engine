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
#include "shader_compiler.h"

#include <set>

namespace ShaderLoader
{
    const std::string INSTANCING_DEFINE = "_INSTANCING";

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

    std::string GetDefinesHash(const std::vector<std::string>& defines)
    {
        std::set<std::string> orderedDefines;
        for (const std::string& define : defines)
            orderedDefines.insert(define);

        std::string combinedDefines;
        for (const std::string& define : orderedDefines)
            combinedDefines += define;

        return std::to_string(Hash::FNV1a(combinedDefines));
    }

    bool HasDefine(const std::vector<std::string>& defines, const std::string& define)
    {
        return std::ranges::find(defines, define) != defines.end();
    }

    std::shared_ptr<Shader> LoadCompiledShader(const std::filesystem::path& path, const std::vector<std::string>& defines)
    {
        const std::string definesHash = GetDefinesHash(defines);
        const bool supportInstancing = HasDefine(defines, INSTANCING_DEFINE);

        const std::string backendLiteral = GetBackendLiteral(GraphicsBackend::Current()->GetName());
        const std::filesystem::path compiledShaderPath = FileSystem::GetBuildResourcesPath() / path;
        const std::filesystem::path compiledShaderPermutationPath = compiledShaderPath / backendLiteral / definesHash;

#ifdef RENDER_ENGINE_EDITOR
        const std::filesystem::path editorShaderPath = FileSystem::GetEditorResourcesPath() / (path.string() + ".hlsl");
		const std::filesystem::path dependenciesFilePath = compiledShaderPermutationPath / "dependencies.json";

        if (FileSystem::FileExists(dependenciesFilePath))
        {
            std::unordered_map<std::string, std::filesystem::file_time_type::duration::rep> dependencies;
            ShaderParser::ParseDependencies(FileSystem::ReadFile(dependenciesFilePath), dependencies);

            const std::filesystem::path editorShaderDirPath = editorShaderPath.parent_path();
            for (const auto& pair : dependencies)
            {
                const std::filesystem::path dependencyPath = editorShaderDirPath / pair.first;
                if (!FileSystem::FileExists(dependencyPath) || std::filesystem::last_write_time(dependencyPath).time_since_epoch().count() != pair.second)
                {
                    if (!ShaderCompilerLib::CompileShader(editorShaderPath, compiledShaderPath, backendLiteral, defines, false))
                        return nullptr;
                    break;
                }
            }
        }
        else if (!ShaderCompilerLib::CompileShader(editorShaderPath, compiledShaderPath, backendLiteral, defines, false))
            return nullptr;
#endif

        const std::filesystem::path reflectionPath = compiledShaderPermutationPath / "reflection.json";
        if (!FileSystem::FileExists(reflectionPath))
            return nullptr;

        const std::string reflectionJson = FileSystem::ReadFile(reflectionPath);
        std::unordered_map<std::string, GraphicsBackendTextureInfo> textures;
        std::unordered_map<std::string, GraphicsBackendSamplerInfo> samplers;
        std::unordered_map<std::string, std::shared_ptr<GraphicsBackendBufferInfo>> buffers;
        std::unordered_map<std::string, GraphicsBackendTLASInfo> TLASes;
        ThreadGroupSize threadGroupSize;
        ShaderParser::ParseReflection(reflectionJson, textures, buffers, samplers, TLASes, threadGroupSize);

        std::string shaderDebugName;
        shaderDebugName.append(path.string());
        shaderDebugName.append("_");
        shaderDebugName.append(definesHash);

        std::vector<GraphicsBackendShaderObject> shaders;
        for (int i = 0; i < static_cast<int>(ShaderType::COUNT); ++i)
        {
            const ShaderType shaderType = static_cast<ShaderType>(i);
            const std::string& shaderFilename = GraphicsBackendBase::GetShaderTypeName(shaderType);

            std::filesystem::path sourcePath = compiledShaderPermutationPath / shaderFilename;
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

        if (shaders.empty())
            return nullptr;

        return std::make_shared<Shader>(shaders, textures, buffers, samplers, TLASes, threadGroupSize, shaderDebugName, supportInstancing);
    }

	std::shared_ptr<Shader> Load(const std::filesystem::path& path, const std::vector<std::string>& defines)
    {
        std::shared_ptr<Shader> shader = LoadCompiledShader(path, defines);

        if (!shader)
        {
            std::string definesString = defines.empty() ? "<no defines>" : "";
            for (const std::string& define : defines)
                definesString += define + " ";

            Debug::LogErrorFormat("[ShaderLoader] Can't load shader {}\n{}", path.string(), definesString);
            return nullptr;
        }

        return shader;
    }
} // namespace ShaderLoader