#include "reflection.h"
#include "serialization.h"

CComPtr<IDxcResult> CompileShader(const std::filesystem::path &hlslPath, const CComPtr<IDxcUtils>& pUtils, const CComPtr<IDxcCompiler3>& pCompiler,
                   const CComPtr<IDxcIncludeHandler>& pIncludeHandler, bool isVertexShader)
{
    LPCWSTR vszArgs[] =
            {
                    L"-E", (isVertexShader ? L"vertexMain" : L"fragmentMain"),
                    L"-T", (isVertexShader ? L"vs_6_0" : L"ps_6_0"),
                    //L"-Qstrip_reflect",
                    //L"-Zi",
                    //L"-Qembed_debug"
            };

    CComPtr<IDxcBlobEncoding> pSource = nullptr;
    std::wstring hlslPathString = hlslPath.wstring();
    pUtils->LoadFile(hlslPathString.c_str(), nullptr, &pSource);
    DxcBuffer Source{pSource->GetBufferPointer(), pSource->GetBufferSize(), DXC_CP_ACP};

    CComPtr<IDxcResult> pResults;
    pCompiler->Compile(&Source, vszArgs, _countof(vszArgs), pIncludeHandler, IID_PPV_ARGS(&pResults));

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

void WriteShaderBinary(const std::filesystem::path &hlslPath, const CComPtr<IDxcResult>& results, bool isVertexShader)
{
    CComPtr<IDxcBlob> pShader = nullptr;
    results->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&pShader), nullptr);
    if (pShader != nullptr)
    {
        std::filesystem::path outputPath = hlslPath.parent_path() / "output" / (isVertexShader ? "vs.bin" : "ps.bin");
        std::filesystem::create_directory(outputPath.parent_path());

        FILE *fp = fopen(outputPath.c_str(), "wb");
        fwrite(pShader->GetBufferPointer(), pShader->GetBufferSize(), 1, fp);
        fclose(fp);
    }
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        std::cout << "No HLSL path specified" << std::endl;
        return 1;
    }

    std::filesystem::path hlslPath = std::filesystem::absolute(std::filesystem::path(argv[1]));
    std::cout << "Compiling shader at path: " << hlslPath << std::endl;

    CComPtr<IDxcUtils> pUtils;
    CComPtr<IDxcCompiler3> pCompiler;
    DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&pUtils));
    DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&pCompiler));

    CComPtr<IDxcIncludeHandler> pIncludeHandler;
    pUtils->CreateDefaultIncludeHandler(&pIncludeHandler);

    CComPtr<IDxcResult> vertexResults = CompileShader(hlslPath, pUtils, pCompiler, pIncludeHandler, true);
    CComPtr<IDxcResult> fragmentResults = CompileShader(hlslPath, pUtils, pCompiler, pIncludeHandler, false);

    WriteShaderBinary(hlslPath, vertexResults, true);
    WriteShaderBinary(hlslPath, fragmentResults, false);

    Reflection reflection;
    ExtractReflection(vertexResults, pUtils, true, reflection);
    ExtractReflection(fragmentResults, pUtils, false, reflection);
    WriteReflection(hlslPath, reflection);

    return 0;
}