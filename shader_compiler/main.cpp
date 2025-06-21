#include "reflection.h"
#include "serialization.h"
#include "graphics_backend.h"
#include "defines.h"
#include "arguments.h"

CComPtr<IDxcResult> CompileDXC(const std::filesystem::path &hlslPath, const CComPtr<IDxcUtils>& pUtils, const CComPtr<IDxcCompiler3>& pCompiler,
                               const CComPtr<IDxcIncludeHandler>& pIncludeHandler, GraphicsBackend backend,
                               const std::vector<std::wstring>& defines, bool isVertexShader)
{
    std::wstring pathWString = hlslPath.parent_path().wstring();
    std::wstring backendDefine = GetBackendDefine(backend);

    std::vector<LPCWSTR> vszArgs;
    if (backend != GRAPHICS_BACKEND_DX12)
        vszArgs.push_back(L"-spirv");
    if (Arguments::Contains("-debug"))
    {
        vszArgs.push_back(L"-Zi");
        vszArgs.push_back(L"-Qembed_debug");
    }
    vszArgs.push_back(L"-E");
    vszArgs.push_back(isVertexShader ? L"vertexMain" : L"fragmentMain");
    vszArgs.push_back(L"-T");
    vszArgs.push_back(isVertexShader ? L"vs_6_0" : L"ps_6_0");
    vszArgs.push_back(L"-D");
    vszArgs.push_back(backendDefine.c_str());
    vszArgs.push_back(L"-I");
    vszArgs.push_back(pathWString.c_str());

    for (const auto& define : defines)
    {
        vszArgs.push_back(L"-D");
        vszArgs.push_back(define.c_str());
    }

    CComPtr<IDxcBlobEncoding> pSource = nullptr;
    std::wstring hlslPathString = hlslPath.wstring();
    pUtils->LoadFile(hlslPathString.c_str(), nullptr, &pSource);
    DxcBuffer Source{pSource->GetBufferPointer(), pSource->GetBufferSize(), DXC_CP_ACP};

    CComPtr<IDxcResult> pResults;
    pCompiler->Compile(&Source, vszArgs.data(), vszArgs.size(), pIncludeHandler, IID_PPV_ARGS(&pResults));

    CComPtr<IDxcBlobUtf8> pErrors = nullptr;
    pResults->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&pErrors), nullptr);
    if (pErrors != nullptr && pErrors->GetStringLength() != 0)
    {
        std::cout << "Warnings and Errors:\n" << pErrors->GetStringPointer() << std::endl;
    }

    HRESULT hrStatus;
    pResults->GetStatus(&hrStatus);
    if (FAILED(hrStatus))
    {
        std::cout << "Compilation Failed" << std::endl;
        return nullptr;
    }

    return pResults;
}

spirv_cross::Compiler* CompileSPIRV(const CComPtr<IDxcResult>& dxcResult, GraphicsBackend backend, bool isVertexShader)
{
    CComPtr<IDxcBlob> pShader = nullptr;
    dxcResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&pShader), nullptr);

    auto sourceBuffer = static_cast<uint32_t*>(pShader->GetBufferPointer());
    size_t sourceSize = pShader->GetBufferSize() * sizeof(uint8_t) / sizeof(uint32_t);

    bool isGLES = backend == GRAPHICS_BACKEND_GLES;
    if (backend == GRAPHICS_BACKEND_OPENGL || isGLES)
    {
        auto glsl = new spirv_cross::CompilerGLSL(sourceBuffer, sourceSize);
        glsl->build_combined_image_samplers();

        spirv_cross::CompilerGLSL::Options options;
        options.version = isGLES ? 320 : 460;
        options.es = isGLES;
        glsl->set_common_options(options);

        const auto& combinedImageSamplers = glsl->get_combined_image_samplers();
        for (int i = 0; i < combinedImageSamplers.size(); i++)
        {
            const auto& sampler = combinedImageSamplers[i];
            glsl->set_name(sampler.combined_id, glsl->get_name(sampler.image_id));
            glsl->set_decoration(sampler.combined_id, spv::DecorationLocation, i);
        }

        return glsl;
    }

    if (backend == GRAPHICS_BACKEND_METAL)
    {
        auto msl = new spirv_cross::CompilerMSL(sourceBuffer, sourceSize);

        spirv_cross::CompilerMSL::Options options;
        options.platform = spirv_cross::CompilerMSL::Options::macOS;
        options.msl_version = spirv_cross::CompilerMSL::Options::make_msl_version(3);
        msl->set_msl_options(options);

        return msl;
    }

    return nullptr;
}

void WriteShaderBinary(const std::filesystem::path &outputDirPath, const CComPtr<IDxcResult>& results, bool isVertexShader)
{
    CComPtr<IDxcBlob> pShader = nullptr;
    results->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&pShader), nullptr);
    if (pShader != nullptr)
    {
        std::filesystem::path outputPath = outputDirPath / (isVertexShader ? "vs" : "ps");
        std::filesystem::create_directories(outputPath.parent_path());

        FILE *fp = fopen(outputPath.string().c_str(), "wb");
        fwrite(pShader->GetBufferPointer(), pShader->GetBufferSize(), 1, fp);
        fclose(fp);
    }
}

void WriteShaderSource(const std::filesystem::path& outputDirPath, spirv_cross::Compiler* compiler, bool isVertexShader)
{
    if (!compiler)
        return;

    std::filesystem::path outputPath = outputDirPath / (isVertexShader ? "vs" : "ps");
    std::filesystem::create_directories(outputPath.parent_path());

    std::string shaderSource;
    try
    {
        shaderSource = compiler->compile();
    }
    catch (std::exception e)
    {
        std::cout << e.what() << std::endl;
    }

    FILE *fp = fopen(outputPath.string().c_str(), "w");
    fwrite(shaderSource.c_str(), shaderSource.size(), 1, fp);
    fclose(fp);
}

int main(int argc, char **argv)
{
    Arguments::Init(argv, argc);

    if (!Arguments::Contains("-backend") || !Arguments::Contains("-output") || !Arguments::Contains("-input"))
    {
        std::cout << "No HLSL path or no target backend are specified" << std::endl;
        return 1;
    }

    GraphicsBackend backend;
    if (!TryGetBackend(Arguments::Get("-backend"), backend))
    {
        std::cout << "Unknown target backend. Supported options:";
        for (int i = 0; i < GRAPHICS_BACKEND_MAX; ++i)
        {
            std::cout << "\n\t- " << GetBackendLiteral(static_cast<GraphicsBackend>(i));
        }
        std::cout << std::endl;
        return 1;
    }

    std::filesystem::path hlslPath = std::filesystem::absolute(std::filesystem::path(Arguments::Get("-input")));
    std::cout << "Compiling shader at path: " << hlslPath << std::endl;

    std::vector<std::wstring> defines = GetDefines(Arguments::Get("-defines"));
    std::string definesHash = GetDefinesHash(defines);
    PrintDefines(defines, definesHash);

    CComPtr<IDxcUtils> pUtils;
    CComPtr<IDxcCompiler3> pCompiler;
    DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&pUtils));
    DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&pCompiler));

    CComPtr<IDxcIncludeHandler> pIncludeHandler;
    pUtils->CreateDefaultIncludeHandler(&pIncludeHandler);

    CComPtr<IDxcResult> vertexDXC = CompileDXC(hlslPath, pUtils, pCompiler, pIncludeHandler, backend, defines, true);
    CComPtr<IDxcResult> fragmentDXC = CompileDXC(hlslPath, pUtils, pCompiler, pIncludeHandler, backend, defines, false);

    Reflection reflection;
    std::filesystem::path outputDirPath = std::filesystem::path(Arguments::Get("-output")) / GetBackendLiteral(backend) / definesHash;

    if (backend == GRAPHICS_BACKEND_DX12)
    {
        WriteShaderBinary(outputDirPath, vertexDXC, true);
        WriteShaderBinary(outputDirPath, fragmentDXC, false);

        ExtractReflectionFromDXC(vertexDXC, pUtils, true, reflection);
        ExtractReflectionFromDXC(fragmentDXC, pUtils, false, reflection);
    }
    else
    {
        spirv_cross::Compiler *vertexSPIRV = CompileSPIRV(vertexDXC, backend, true);
        spirv_cross::Compiler *fragmentSPIRV = CompileSPIRV(fragmentDXC, backend, false);

        WriteShaderSource(outputDirPath, vertexSPIRV, true);
        WriteShaderSource(outputDirPath, fragmentSPIRV, false);

        ExtractReflectionFromSPIRV(vertexSPIRV, true, reflection, backend);
        ExtractReflectionFromSPIRV(fragmentSPIRV, false, reflection, backend);
    }

    WriteReflection(outputDirPath, reflection);

    return 0;
}