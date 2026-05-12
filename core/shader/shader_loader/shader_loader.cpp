#include "shader_loader.h"
#include "shader/shader.h"
#include "debug.h"
#include "graphics/graphics.h"
#include "file_system.h"
#include "enums/shader_type.h"
#include "types/graphics_backend_shader_object.h"
#include "graphics_backend_api.h"
#include "shader_parser.h"
#ifdef RENDER_ENGINE_EDITOR
#include "shader_compiler.h"
#endif
#include "arguments.h"

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

    bool HasDefine(const std::vector<std::string>& defines, const std::string& define)
    {
        return std::ranges::find(defines, define) != defines.end();
    }

    bool LoadCompiledShader(const std::filesystem::path& path, const std::vector<std::string>& defines,
        std::vector<GraphicsBackendShaderObject>& outShaders,
        std::unordered_map<std::string, GraphicsBackendTextureInfo>& outTextures,
        std::unordered_map<std::string, std::shared_ptr<GraphicsBackendBufferInfo>>& outBuffers,
        std::unordered_map<std::string, GraphicsBackendSamplerInfo>& outSamplers,
        std::unordered_map<std::string, GraphicsBackendTLASInfo>& outTLASes,
        ThreadGroupSize& outThreadGroupSize,
        std::string& outName,
        bool& outSupportInstancing)
    {
        const bool shaderDebug = Arguments::Contains("-shaderdebug");

        const std::string shaderHash = GetShaderHash(GetDefinesHash(defines), shaderDebug);
        outSupportInstancing = HasDefine(defines, INSTANCING_DEFINE);

        const std::string backendLiteral = GetBackendLiteral(GraphicsBackend::Current()->GetName());
        const std::filesystem::path compiledShaderPath = FileSystem::GetBuildResourcesPath() / path;
        const std::filesystem::path compiledShaderPermutationPath = compiledShaderPath / backendLiteral / shaderHash;

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
                    if (!ShaderCompilerLib::CompileShader(editorShaderPath, compiledShaderPath, backendLiteral, defines, shaderDebug))
                        return false;
                    break;
                }
            }
        }
        else if (!ShaderCompilerLib::CompileShader(editorShaderPath, compiledShaderPath, backendLiteral, defines, shaderDebug))
            return false;
#endif

        const std::filesystem::path reflectionPath = compiledShaderPermutationPath / "reflection.json";
        if (!FileSystem::FileExists(reflectionPath))
            return false;

        const std::string reflectionJson = FileSystem::ReadFile(reflectionPath);
        ShaderParser::ParseReflection(reflectionJson, outTextures, outBuffers, outSamplers, outTLASes, outThreadGroupSize);

        outName = path.string();
        outName.append("_");
        outName.append(shaderHash);

        std::vector<GraphicsBackendShaderObject> shaders;
        for (int i = 0; i < static_cast<int>(ShaderType::COUNT); ++i)
        {
            const ShaderType shaderType = static_cast<ShaderType>(i);
            const std::string& shaderFilename = GraphicsBackendBase::GetShaderTypeName(shaderType);

            std::filesystem::path sourcePath = compiledShaderPermutationPath / shaderFilename;
            if (!FileSystem::FileExists(sourcePath))
                continue;

            std::string shaderFunctionDebugName = outName;
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
            outShaders.push_back(shader);
        }

        if (outShaders.empty())
            return false;

        return true;
    }

    std::string GetShaderHash(size_t definesHash, bool debug)
    {
        return std::to_string(Hash::Combine(definesHash, debug ? 1 : 0));
    }

	void Load(const std::filesystem::path& path, const std::vector<std::string>& defines,
              std::vector<GraphicsBackendShaderObject>& outShaders,
              std::unordered_map<std::string, GraphicsBackendTextureInfo>& outTextures,
              std::unordered_map<std::string, std::shared_ptr<GraphicsBackendBufferInfo>>& outBuffers,
              std::unordered_map<std::string, GraphicsBackendSamplerInfo>& outSamplers,
              std::unordered_map<std::string, GraphicsBackendTLASInfo>& outTLASes,
              ThreadGroupSize& outThreadGroupSize,
              std::string& outName,
              bool& outSupportInstancing)
    {
        const bool success = LoadCompiledShader(path, defines, outShaders, outTextures, outBuffers, outSamplers, outTLASes, outThreadGroupSize, outName, outSupportInstancing);

        if (!success)
        {
            std::string definesString = defines.empty() ? "<no defines>" : "";
            for (const std::string& define : defines)
                definesString += define + " ";

            Debug::LogErrorFormat("[ShaderLoader] Can't load shader {}\n{}", path.string(), definesString);
        }
    }
} // namespace ShaderLoader