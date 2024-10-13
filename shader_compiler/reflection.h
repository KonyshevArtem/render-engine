#ifndef RENDER_ENGINE_SHADER_COMPILER_REFLECTION_H
#define RENDER_ENGINE_SHADER_COMPILER_REFLECTION_H

#include "graphics_backend.h"

#include "dxcapi.h"
#include "d3d12shader.h"

#include "spirv_glsl.hpp"
#include "spirv_msl.hpp"

#include <string>
#include <iostream>
#include <unordered_map>

struct Bindings
{
    int32_t Vertex = -1;
    int32_t Fragment = -1;
};

template<typename T>
concept HasBindings = requires(T t) {
    { t.Bindings } -> std::same_as<Bindings&>;
};

struct BufferDesc
{
    uint32_t Size;
    Bindings Bindings;
    std::unordered_map<std::string, uint32_t> Variables;
};

struct GenericDesc
{
    Bindings Bindings;
};

struct Reflection
{
    std::unordered_map<std::string, BufferDesc> Buffers;
    std::unordered_map<std::string, GenericDesc> Textures;
    std::unordered_map<std::string, GenericDesc> Samplers;
};

void SetBinding(Bindings& bindings, int bindPoint, bool isVertexShader)
{
    int32_t* binding = isVertexShader ? &bindings.Vertex : &bindings.Fragment;
    *binding = bindPoint;
}

template<HasBindings T>
bool TrySetBinding(std::unordered_map<std::string, T>& descriptions, const std::string& name, int bindPoint, bool isVertexShader)
{
    auto it = descriptions.find(name);
    if (it != descriptions.end())
    {
        T& bufferDesc = it->second;
        SetBinding(bufferDesc.Bindings, bindPoint, isVertexShader);
        return true;
    }

    return false;
}

int32_t ExtractBindingSPIRV(spirv_cross::Compiler* compiler, spirv_cross::ID resourceID, int index, GraphicsBackend backend)
{
    if (backend == GRAPHICS_BACKEND_METAL)
    {
        auto msl = reinterpret_cast<spirv_cross::CompilerMSL*>(compiler);
        return msl->get_automatic_msl_resource_binding(resourceID);
    }

    if (compiler->has_decoration(resourceID, spv::DecorationBinding))
        return compiler->get_decoration(resourceID, spv::DecorationBinding);

    return index;
}

void GetSPIRVResourceInfo(spirv_cross::Compiler* compiler, const spirv_cross::Resource& resource, int index, GraphicsBackend backend, std::string& outResourceName, int32_t& outBindPoint)
{
    outResourceName = compiler->get_name(resource.id);
    outBindPoint = ExtractBindingSPIRV(compiler, resource.id, index, backend);
}

void HandleConstantBufferReflection(const _D3D12_SHADER_INPUT_BIND_DESC& inputDesc, const CComPtr<ID3D12ShaderReflection>& reflection,
                                    std::unordered_map<std::string, BufferDesc>& buffers, bool isVertexShader)
{
    if (!TrySetBinding(buffers, inputDesc.Name, inputDesc.BindPoint, isVertexShader))
    {
        ID3D12ShaderReflectionConstantBuffer* bufferReflection = reflection->GetConstantBufferByName(inputDesc.Name);

        _D3D12_SHADER_BUFFER_DESC shaderBufferDesc{};
        bufferReflection->GetDesc(&shaderBufferDesc);

        BufferDesc bufferDesc{shaderBufferDesc.Size};
        SetBinding(bufferDesc.Bindings, inputDesc.BindPoint, isVertexShader);

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

void HandleConstantBufferReflection(spirv_cross::Compiler* compiler, const spirv_cross::Resource& resource, int index, std::unordered_map<std::string, BufferDesc>& buffers, bool isVertexShader, GraphicsBackend backend)
{
    int32_t bindPoint;
    std::string bufferName;
    GetSPIRVResourceInfo(compiler, resource, index, backend, bufferName, bindPoint);

    if (!TrySetBinding(buffers, bufferName, bindPoint, isVertexShader))
    {
        auto& buffer_type = compiler->get_type(resource.base_type_id);
        auto size = static_cast<uint32_t>(compiler->get_declared_struct_size(buffer_type));

        BufferDesc bufferDesc{size};
        SetBinding(bufferDesc.Bindings, bindPoint, isVertexShader);

        for (uint32_t i = 0; i < buffer_type.member_types.size(); ++i)
        {
            const std::string& varName = compiler->get_member_name(resource.base_type_id, i);
            uint32_t varOffset = compiler->type_struct_member_offset(buffer_type, i);

            bufferDesc.Variables[varName] = varOffset;
        }

        buffers[bufferName] = std::move(bufferDesc);
    }
}

void HandleGenericReflection(const _D3D12_SHADER_INPUT_BIND_DESC &inputDesc, std::unordered_map<std::string, GenericDesc> &resources, bool isVertexShader)
{
    if (!TrySetBinding(resources, inputDesc.Name, inputDesc.BindPoint, isVertexShader))
    {
        GenericDesc desc;
        SetBinding(desc.Bindings, inputDesc.BindPoint, isVertexShader);

        resources[inputDesc.Name] = desc;
    }
}

void HandleGenericReflection(const std::string& resourceName, int32_t bindPoint, std::unordered_map<std::string, GenericDesc> &resources, bool isVertexShader)
{
    if (!TrySetBinding(resources, resourceName, bindPoint, isVertexShader))
    {
        GenericDesc desc;
        SetBinding(desc.Bindings, bindPoint, isVertexShader);

        resources[resourceName] = desc;
    }
}

void HandleGenericReflection(spirv_cross::Compiler* compiler, const spirv_cross::Resource& resource, int index, std::unordered_map<std::string, GenericDesc> &resources, bool isVertexShader, GraphicsBackend backend)
{
    int32_t bindPoint;
    std::string resourceName;
    GetSPIRVResourceInfo(compiler, resource, index, backend, resourceName, bindPoint);

    HandleGenericReflection(resourceName, bindPoint, resources, isVertexShader);
}

void HandleCombinedImageSamplerReflection(spirv_cross::Compiler* compiler, const spirv_cross::Resource& resource, int index, std::unordered_map<std::string, GenericDesc> &textures, std::unordered_map<std::string, GenericDesc> &samplers, bool isVertexShader, GraphicsBackend backend)
{
    int32_t bindPoint;
    std::string textureName;
    GetSPIRVResourceInfo(compiler, resource, index, backend, textureName, bindPoint);

    HandleGenericReflection(textureName, bindPoint, textures, isVertexShader);
    HandleGenericReflection("sampler" + textureName, bindPoint, samplers, isVertexShader);
}

void ExtractReflectionFromDXC(const CComPtr<IDxcResult> &results, const CComPtr<IDxcUtils> &utils, bool isVertexShader, Reflection& reflection)
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
                HandleGenericReflection(inputDesc, reflection.Textures, isVertexShader);
            }
            else if (inputDesc.Type == D3D_SIT_SAMPLER)
            {
                HandleGenericReflection(inputDesc, reflection.Samplers, isVertexShader);
            }
        }
    }
}

void ExtractReflectionFromSPIRV(spirv_cross::Compiler* compiler, bool isVertexShader, Reflection& reflection, GraphicsBackend backend)
{
    spirv_cross::ShaderResources resources = compiler->get_shader_resources();

    for (int i = 0; i < resources.uniform_buffers.size(); ++i)
    {
        HandleConstantBufferReflection(compiler, resources.uniform_buffers[i], i, reflection.Buffers, isVertexShader, backend);
    }

    if (resources.sampled_images.empty())
    {
        for (int i = 0; i < resources.separate_images.size(); ++i)
        {
            HandleGenericReflection(compiler, resources.separate_images[i], i, reflection.Textures, isVertexShader, backend);
        }

        for (int i = 0; i < resources.separate_samplers.size(); ++i)
        {
            HandleGenericReflection(compiler, resources.separate_samplers[i], i, reflection.Samplers, isVertexShader, backend);
        }
    }
    else
    {
        for (int i = 0; i < resources.sampled_images.size(); ++i)
        {
            HandleCombinedImageSamplerReflection(compiler, resources.sampled_images[i], i, reflection.Textures, reflection.Samplers, isVertexShader, backend);
        }
    }
}

#endif //RENDER_ENGINE_SHADER_COMPILER_REFLECTION_H
