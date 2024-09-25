#ifndef RENDER_ENGINE_SHADER_COMPILER_REFLECTION_H
#define RENDER_ENGINE_SHADER_COMPILER_REFLECTION_H

#include "dxcapi.h"
#include "d3d12shader.h"

#include <string>
#include <iostream>
#include <filesystem>
#include <unordered_map>

struct Bindings
{
    int32_t Vertex = -1;
    int32_t Fragment = -1;
};

struct BufferDesc
{
    uint32_t Size;
    Bindings Bindings;
    std::unordered_map<std::string, uint32_t> Variables;
};

struct TextureDesc
{
    Bindings Bindings;
    bool HasSampler;
};

struct Reflection
{
    std::unordered_map<std::string, BufferDesc> Buffers;
    std::unordered_map<std::string, TextureDesc> Textures;
};

void HandleConstantBufferReflection(const _D3D12_SHADER_INPUT_BIND_DESC& inputDesc, const CComPtr<ID3D12ShaderReflection>& reflection,
                                    std::unordered_map<std::string, BufferDesc>& buffers, bool isVertexShader)
{
    auto SetBinding = [&inputDesc, isVertexShader](BufferDesc& bufferDesc)
    {
        int32_t* binding = isVertexShader ? &bufferDesc.Bindings.Vertex : &bufferDesc.Bindings.Fragment;
        *binding = static_cast<int32_t>(inputDesc.BindPoint);
    };

    auto it = buffers.find(inputDesc.Name);
    if (it != buffers.end())
    {
        BufferDesc& bufferDesc = it->second;
        SetBinding(bufferDesc);
    }
    else
    {
        ID3D12ShaderReflectionConstantBuffer* bufferReflection = reflection->GetConstantBufferByName(inputDesc.Name);

        _D3D12_SHADER_BUFFER_DESC shaderBufferDesc{};
        bufferReflection->GetDesc(&shaderBufferDesc);

        BufferDesc bufferDesc{shaderBufferDesc.Size};
        SetBinding(bufferDesc);

        for (int i = 0; i < shaderBufferDesc.Variables; ++i)
        {
            ID3D12ShaderReflectionVariable* varReflection = bufferReflection->GetVariableByIndex(i);

            _D3D12_SHADER_VARIABLE_DESC varDesc{};
            varReflection->GetDesc(&varDesc);

            bufferDesc.Variables[varDesc.Name] = varDesc.StartOffset;
        }

        buffers[inputDesc.Name] = std::move(bufferDesc);
    }
}

void HandleTextureReflection(const _D3D12_SHADER_INPUT_BIND_DESC &inputDesc, std::unordered_map<std::string, TextureDesc> &textures, bool isVertexShader)
{
    auto SetBinding = [&inputDesc, isVertexShader](TextureDesc& textureDesc)
    {
        int32_t* binding = isVertexShader ? &textureDesc.Bindings.Vertex : &textureDesc.Bindings.Fragment;
        *binding = static_cast<int32_t>(inputDesc.BindPoint);
    };

    auto it = textures.find(inputDesc.Name);
    if (it != textures.end())
    {
        TextureDesc& textureDesc = it->second;
        SetBinding(textureDesc);
    }
    else
    {
        TextureDesc textureDesc;
        SetBinding(textureDesc);

        textures[inputDesc.Name] = textureDesc;
    }
}

void HandleSamplerReflection(const _D3D12_SHADER_INPUT_BIND_DESC &inputDesc, std::unordered_map<std::string, TextureDesc> &textures, bool isVertexShader)
{
    auto it = std::find_if(textures.begin(), textures.end(), [&inputDesc, isVertexShader](const std::pair<std::string, TextureDesc>& pair)
    {
        uint32_t bindPoint = isVertexShader ? pair.second.Bindings.Vertex : pair.second.Bindings.Fragment;
        return inputDesc.BindPoint == bindPoint;
    });

    if (it == textures.end())
    {
        std::cout << "Texture with corresponding binding point not found for sampler: " << inputDesc.Name << " " << inputDesc.BindPoint << std::endl;
        return;
    }

    TextureDesc& textureDesc = it->second;
    textureDesc.HasSampler = true;
}

void ExtractReflection(const CComPtr<IDxcResult> &results, const CComPtr<IDxcUtils> &utils, bool isVertexShader, Reflection& reflection)
{
    CComPtr<IDxcBlob> pReflectionData;
    results->GetOutput(DXC_OUT_REFLECTION, IID_PPV_ARGS(&pReflectionData), nullptr);
    if (pReflectionData != nullptr)
    {
        DxcBuffer reflectionData{pReflectionData->GetBufferPointer(), pReflectionData->GetBufferSize(), DXC_CP_ACP};

        CComPtr<ID3D12ShaderReflection> pReflection;
        utils->CreateReflection(&reflectionData, IID_PPV_ARGS(&pReflection));

        D3D12_SHADER_DESC shaderDesc;
        pReflection->GetDesc(&shaderDesc);

        for (int i = 0; i < shaderDesc.BoundResources; ++i)
        {
            _D3D12_SHADER_INPUT_BIND_DESC inputDesc{};
            pReflection->GetResourceBindingDesc(i, &inputDesc);

            if (inputDesc.Type == D3D_SIT_CBUFFER)
            {
                HandleConstantBufferReflection(inputDesc, pReflection, reflection.Buffers, isVertexShader);
            }
            else if (inputDesc.Type == D3D_SIT_TEXTURE)
            {
                HandleTextureReflection(inputDesc, reflection.Textures, isVertexShader);
            }
            else if (inputDesc.Type == D3D_SIT_SAMPLER)
            {
                HandleSamplerReflection(inputDesc, reflection.Textures, isVertexShader);
            }
        }
    }
}

#endif //RENDER_ENGINE_SHADER_COMPILER_REFLECTION_H
