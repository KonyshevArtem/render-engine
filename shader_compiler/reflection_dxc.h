#ifndef RENDER_ENGINE_SHADER_COMPILER_REFLECTION_DXC_H
#define RENDER_ENGINE_SHADER_COMPILER_REFLECTION_DXC_H

#include "graphics_backend.h"
#include "reflection_common.h"

#include "dxcapi.h"
#include "d3d12shader.h"

#if __has_include("atlbase.h")
#include <atlbase.h>
#endif

namespace DXCReflection_Local
{
    inline bool IsReadWriteResource(D3D_SHADER_INPUT_TYPE type)
    {
        return type == D3D_SIT_UAV_RWTYPED || type == D3D_SIT_UAV_RWBYTEADDRESS || type == D3D_SIT_UAV_RWSTRUCTURED;
    }

    inline void WriteConstantBufferReflection(const std::string& name, UINT bindPoint, const CComPtr<ID3D12ShaderReflection>& reflection,
        std::unordered_map<std::string, BufferDescriptor>& buffers)
    {
        if (buffers.contains(name))
            return;

        ID3D12ShaderReflectionConstantBuffer* bufferReflection = reflection->GetConstantBufferByName(name.c_str());

        _D3D12_SHADER_BUFFER_DESC shaderBufferDesc{};
        bufferReflection->GetDesc(&shaderBufferDesc);

        BufferDescriptor bufferDesc{ bindPoint, shaderBufferDesc.Size, BufferType::CONSTANT_BUFFER, false };

        for (int i = 0; i < shaderBufferDesc.Variables; ++i)
        {
            ID3D12ShaderReflectionVariable* varReflection = bufferReflection->GetVariableByIndex(i);

            _D3D12_SHADER_VARIABLE_DESC varDesc{};
            varReflection->GetDesc(&varDesc);

            bufferDesc.Variables[varDesc.Name] = varDesc.StartOffset;
        }

        buffers[name] = std::move(bufferDesc);
    }
}

inline void ExtractReflectionFromDXC(const CComPtr<IDxcResult>& results, const CComPtr<IDxcUtils>& utils, Reflection& reflection, ShaderType shaderType)
{
    CComPtr<IDxcBlob> pReflectionData;
    results->GetOutput(DXC_OUT_REFLECTION, IID_PPV_ARGS(&pReflectionData), nullptr);
    if (pReflectionData != nullptr)
    {
        const DxcBuffer reflectionData{ pReflectionData->GetBufferPointer(), pReflectionData->GetBufferSize(), DXC_CP_ACP };

        CComPtr<ID3D12ShaderReflection> pReflection;
        utils->CreateReflection(&reflectionData, IID_PPV_ARGS(&pReflection));

        D3D12_SHADER_DESC shaderDesc;
        pReflection->GetDesc(&shaderDesc);

        for (int i = 0; i < shaderDesc.BoundResources; ++i)
        {
            _D3D12_SHADER_INPUT_BIND_DESC inputDesc{};
            pReflection->GetResourceBindingDesc(i, &inputDesc);

            const std::string name = inputDesc.Name;
            const UINT bindPoint = inputDesc.BindPoint;
            const D3D_SHADER_INPUT_TYPE type = inputDesc.Type;
            const bool readWrite = DXCReflection_Local::IsReadWriteResource(type);

            if (type == D3D_SIT_CBUFFER)
                DXCReflection_Local::WriteConstantBufferReflection(name, bindPoint, pReflection, reflection.Buffers);
            else if (type == D3D_SIT_TEXTURE || type == D3D_SIT_UAV_RWTYPED)
            {
                if (inputDesc.Dimension == D3D_SRV_DIMENSION_BUFFER)
                    WriteBufferDescriptor(name, bindPoint, 0, BufferType::TYPED_BUFFER, readWrite, reflection.Buffers);
                else
                    WriteTextureDescriptor(name, bindPoint, readWrite, reflection.Textures);
            }
            else if (type == D3D_SIT_SAMPLER)
                WriteResourceDescriptor(name, bindPoint, reflection.Samplers);
            else if (type == D3D_SIT_STRUCTURED || type == D3D_SIT_UAV_RWSTRUCTURED)
                WriteBufferDescriptor(name, bindPoint, inputDesc.NumSamples, BufferType::STRUCTURED_BUFFER, readWrite, reflection.Buffers);
            else if (type == D3D_SIT_BYTEADDRESS || type == D3D_SIT_UAV_RWBYTEADDRESS)
                WriteBufferDescriptor(name, bindPoint, 0, BufferType::BYTE_ADDRESS_BUFFER, readWrite, reflection.Buffers);
        }

        if (shaderType == ShaderType::COMPUTE_SHADER)
	        pReflection->GetThreadGroupSize(&reflection.ThreadGroupSize.X, &reflection.ThreadGroupSize.Y, &reflection.ThreadGroupSize.Z);
    }
}

#endif //RENDER_ENGINE_SHADER_COMPILER_REFLECTION_DXC_H
