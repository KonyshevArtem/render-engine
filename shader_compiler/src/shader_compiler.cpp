#include "shader_compiler.h"
#include "reflection_spirv.h"
#include "reflection_dxc.h"
#include "serialization.h"
#include "graphics_backend.h"
#include "include_handler.h"
#include "defines.h"
#include "debug.h"

#include <fstream>
#include <unordered_set>
#include <iostream>

namespace ShaderCompilerLib
{
    std::string ReadFile(const std::filesystem::path& path)
    {
        std::ifstream file(path, std::ios::binary);
        return std::string(
            std::istreambuf_iterator<char>(file),
            std::istreambuf_iterator<char>());
    }

    CComPtr<IDxcResult> CompileDXC(const std::filesystem::path& hlslPath, const std::filesystem::path& outputPath, const CComPtr<IDxcUtils>& pUtils, const CComPtr<IDxcCompiler3>& pCompiler,
        const CComPtr<IDxcIncludeHandler>& pIncludeHandler, GraphicsBackend backend,
        const std::vector<std::wstring>& defines, ShaderType shaderType, bool debug)
    {
        const std::wstring parentPath = hlslPath.parent_path().wstring();
        const std::wstring hlslPathString = hlslPath.wstring();
        const std::wstring backendDefine = GetBackendDefine(backend);
        const std::wstring entryPoint = GetShaderEntryPointWString(shaderType);
        const std::wstring profile = GetShaderProfile(shaderType);
		const std::wstring dependencyFilePath = std::filesystem::absolute(outputPath / L"dependencies.d").wstring();

        std::vector<LPCWSTR> vszArgs;
        if (backend != GRAPHICS_BACKEND_DX12)
            vszArgs.push_back(L"-spirv");
        if (debug)
        {
            vszArgs.push_back(L"-Zi");
            vszArgs.push_back(L"-Qembed_debug");
        }

        vszArgs.push_back(L"-E");
        vszArgs.push_back(entryPoint.c_str());
        vszArgs.push_back(L"-T");
        vszArgs.push_back(profile.c_str());
        vszArgs.push_back(L"-D");
        vszArgs.push_back(backendDefine.c_str());
        vszArgs.push_back(L"-I");
        vszArgs.push_back(parentPath.c_str());

        for (const auto& define : defines)
        {
            vszArgs.push_back(L"-D");
            vszArgs.push_back(define.c_str());
        }

        CComPtr<IDxcBlobEncoding> pSource = nullptr;
        pUtils->LoadFile(hlslPathString.c_str(), nullptr, &pSource);
        const DxcBuffer source{ pSource->GetBufferPointer(), pSource->GetBufferSize(), DXC_CP_ACP };

        CComPtr<IDxcResult> pResults;
        pCompiler->Compile(&source, vszArgs.data(), vszArgs.size(), pIncludeHandler, IID_PPV_ARGS(&pResults));

        CComPtr<IDxcBlobUtf8> pErrors = nullptr;
        pResults->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&pErrors), nullptr);
        if (pErrors != nullptr && pErrors->GetStringLength() != 0)
        {
			const std::string errors(pErrors->GetStringPointer(), pErrors->GetStringLength());
			Debug::LogError(errors);
        }

        HRESULT hrStatus;
        pResults->GetStatus(&hrStatus);
        if (FAILED(hrStatus))
            return nullptr;

        return pResults;
    }

    spirv_cross::Compiler* CompileSPIRV(const CComPtr<IDxcResult>& dxcResult, GraphicsBackend backend, ShaderType shaderType)
    {
        CComPtr<IDxcBlob> pShader = nullptr;
        dxcResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&pShader), nullptr);

        auto sourceBuffer = static_cast<uint32_t*>(pShader->GetBufferPointer());
        size_t sourceSize = pShader->GetBufferSize() * sizeof(uint8_t) / sizeof(uint32_t);

        bool isGLES = backend == GRAPHICS_BACKEND_GLES;
        if (backend == GRAPHICS_BACKEND_OPENGL || isGLES)
        {
            auto glsl = new spirv_cross::CompilerGLSL(sourceBuffer, sourceSize);
            glsl->build_dummy_sampler_for_combined_images();
            glsl->build_combined_image_samplers();

            spirv_cross::CompilerGLSL::Options options;
            options.version = isGLES ? 320 : 460;
            options.es = isGLES;
            glsl->set_common_options(options);

            const auto& combinedImageSamplers = glsl->get_combined_image_samplers();
            for (int i = 0; i < combinedImageSamplers.size(); i++)
            {
                const spirv_cross::CombinedImageSampler& imageSampler = combinedImageSamplers[i];
                const uint32_t binding = SPIRVReflection_Local::GetSPIRVBindPoint(glsl, imageSampler.image_id);

                const std::string& imageName = glsl->get_name(imageSampler.image_id);
                glsl->set_name(imageSampler.combined_id, imageName);
                glsl->set_name(imageSampler.sampler_id, "sampler" + imageName);
                glsl->set_decoration(imageSampler.combined_id, spv::DecorationBinding, binding);
                glsl->set_decoration(imageSampler.combined_id, spv::DecorationLocation, binding);
                glsl->set_decoration(imageSampler.sampler_id, spv::DecorationBinding, binding);
            }

            spirv_cross::ShaderResources resources = glsl->get_shader_resources();
            for (const spirv_cross::Resource& buffer : resources.storage_buffers)
            {
                if (!SPIRVReflection_Local::IsSPIRVBufferWritable(glsl, buffer.id))
                    continue;

                const uint32_t binding = SPIRVReflection_Local::GetSPIRVBindPoint(glsl, buffer.id);
                glsl->set_decoration(buffer.id, spv::DecorationBinding, binding + k_OpenGLRWBuffersBindingOffset);
            }

            for (const spirv_cross::Resource& image : resources.separate_images)
            {
                const spirv_cross::SPIRType& type = glsl->get_type(image.base_type_id);
                if (type.image.dim != spv::DimBuffer)
                    continue;

                const uint32_t binding = SPIRVReflection_Local::GetSPIRVBindPoint(glsl, image.id);
                glsl->set_decoration(image.id, spv::DecorationBinding, binding + k_OpenGLTypedBuffersBindingOffset);
            }

            for (const spirv_cross::Resource& image : resources.storage_images)
            {
                const spirv_cross::SPIRType& type = glsl->get_type(image.type_id);
                if (type.image.dim != spv::DimBuffer)
                    continue;

                const uint32_t binding = SPIRVReflection_Local::GetSPIRVBindPoint(glsl, image.id);
                glsl->set_decoration(image.id, spv::DecorationBinding, binding + k_OpenGLTypedBuffersBindingOffset);
            }

            return glsl;
        }

        if (backend == GRAPHICS_BACKEND_METAL)
        {
            auto msl = new spirv_cross::CompilerMSL(sourceBuffer, sourceSize);

            spirv_cross::CompilerMSL::Options options;
            options.platform = spirv_cross::CompilerMSL::Options::macOS;
            options.msl_version = spirv_cross::CompilerMSL::Options::make_msl_version(3);
            options.enable_decoration_binding = true;
            msl->set_msl_options(options);

            spirv_cross::ShaderResources resources = msl->get_shader_resources();
            for (const spirv_cross::Resource& buffer : resources.uniform_buffers)
            {
                uint32_t binding = msl->get_decoration(buffer.id, spv::DecorationBinding);
                msl->set_decoration(buffer.id, spv::DecorationBinding, binding + k_MetalConstantBufferBindingOffset);
            }

            for (const spirv_cross::Resource& buffer : resources.storage_buffers)
            {
                if (!SPIRVReflection_Local::IsSPIRVBufferWritable(msl, buffer.id))
                    continue;

                const uint32_t binding = SPIRVReflection_Local::GetSPIRVBindPoint(msl, buffer.id);
                msl->set_decoration(buffer.id, spv::DecorationBinding, binding + k_MetalRWResourcesBindingOffset);
            }

            for (const spirv_cross::Resource& image : resources.separate_images)
            {
                const spirv_cross::SPIRType& type = msl->get_type(image.base_type_id);
                if (type.image.dim != spv::DimBuffer)
                    continue;

                const uint32_t binding = SPIRVReflection_Local::GetSPIRVBindPoint(msl, image.id);
                msl->set_decoration(image.id, spv::DecorationBinding, binding + k_MetalTypedBufferBindingOffset);
            }

            for (const spirv_cross::Resource& image : resources.storage_images)
            {
                const spirv_cross::SPIRType& type = msl->get_type(image.type_id);
                uint32_t binding = SPIRVReflection_Local::GetSPIRVBindPoint(msl, image.id) + k_MetalRWResourcesBindingOffset;
                if (type.image.dim == spv::DimBuffer)
                    binding += k_MetalTypedBufferBindingOffset;

                msl->set_decoration(image.id, spv::DecorationBinding, binding);
            }

            return msl;
        }

        return nullptr;
    }

    void WriteShaderBinary(const std::filesystem::path& outputDirPath, const CComPtr<IDxcResult>& results, ShaderType shaderType)
    {
        CComPtr<IDxcBlob> pShader = nullptr;
        results->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&pShader), nullptr);
        if (pShader != nullptr)
        {
            const std::filesystem::path outputPath = outputDirPath / GetShaderOutputFilename(shaderType);
            std::filesystem::create_directories(outputPath.parent_path());

            FILE* fp = fopen(outputPath.string().c_str(), "wb");
            fwrite(pShader->GetBufferPointer(), pShader->GetBufferSize(), 1, fp);
            fclose(fp);
        }
    }

    void WriteShaderSource(const std::filesystem::path& outputDirPath, spirv_cross::Compiler* compiler, ShaderType shaderType)
    {
        if (!compiler)
            return;

        const std::filesystem::path outputPath = outputDirPath / GetShaderOutputFilename(shaderType);
        std::filesystem::create_directories(outputPath.parent_path());

        std::string shaderSource;
        try
        {
            shaderSource = compiler->compile();
        }
        catch (std::exception e)
        {
        }

        FILE* fp = fopen(outputPath.string().c_str(), "w");
        fwrite(shaderSource.c_str(), shaderSource.size(), 1, fp);
        fclose(fp);
    }

    bool CompileShader(const std::filesystem::path& inputPath, const std::filesystem::path& outputPath, const std::string& backendName, const std::vector<std::string>& defines, bool debug)
    {
        GraphicsBackend backend;
        if (!TryGetBackend(backendName, backend))
        {
            std::string supportedBackends;
            for (int i = 0; i < GRAPHICS_BACKEND_MAX; ++i)
	            supportedBackends += "\n\t- " + GetBackendLiteral(static_cast<GraphicsBackend>(i));

			Debug::LogErrorFormat("Unknown backend \"{}\". Supported options:{}", backendName, supportedBackends);
            return false;
        }

        const std::vector<std::wstring> wDefines = ConvertDefines(defines);
        const std::string definesHash = GetDefinesHash(wDefines);

        const std::filesystem::path hlslPath = std::filesystem::absolute(inputPath);
        Debug::LogInfoFormat("Compiling shader: {}\n{}", hlslPath.string(), CombineDefines(defines));

        CComPtr<IDxcUtils> pUtils;
        CComPtr<IDxcCompiler3> pCompiler;
        DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&pUtils));
        DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&pCompiler));

		std::unordered_map<std::string, std::filesystem::file_time_type::duration::rep> includePaths;
		includePaths[hlslPath.filename().string()] = std::filesystem::last_write_time(hlslPath).time_since_epoch().count();

        IncludeHandler includeHandler(hlslPath.parent_path(), pUtils);

        Reflection reflection;
        const std::filesystem::path outputDirPath = outputPath / GetBackendLiteral(backend) / definesHash;

        const std::string hlslText = ReadFile(hlslPath);
        for (int i = 0; i < ShaderType::COUNT; ++i)
        {
            const ShaderType shaderType = static_cast<ShaderType>(i);
            const std::string entryPoint = GetShaderEntryPoint(shaderType);
            if (hlslText.find(entryPoint) == std::string::npos)
                continue;

            CComPtr<IDxcResult> dxc = CompileDXC(hlslPath, outputDirPath, pUtils, pCompiler, &includeHandler, backend, wDefines, shaderType, debug);
            if (!dxc)
                return false;

            if (backend == GRAPHICS_BACKEND_DX12)
            {
                WriteShaderBinary(outputDirPath, dxc, shaderType);
                ExtractReflectionFromDXC(dxc, pUtils, reflection, shaderType);
            }
            else
            {
                spirv_cross::Compiler* SPIRV = CompileSPIRV(dxc, backend, shaderType);

                WriteShaderSource(outputDirPath, SPIRV, shaderType);
                ExtractReflectionFromSPIRV(SPIRV, reflection, backend, shaderType);
            }

			includeHandler.ExtractIncludePaths(includePaths);
        }

        WriteReflection(outputDirPath, reflection);
		WriteIncludeFiles(outputDirPath, includePaths);

        return true;
    }
}