#ifndef RENDER_ENGINE_SHADER_COMPILER_REFLECTION_SPIRV_H
#define RENDER_ENGINE_SHADER_COMPILER_REFLECTION_SPIRV_H

#include "graphics_backend.h"
#include "reflection_common.h"

#include "spirv_glsl.hpp"
#include "spirv_msl.hpp"

namespace SPIRVReflection_Local
{
    inline uint32_t GetSPIRVBindPoint(const spirv_cross::Compiler* compiler, const spirv_cross::ID& resourceID)
    {
        return compiler->get_decoration(resourceID, spv::DecorationBinding);
    }

    inline bool IsSPIRVBufferWritable(const spirv_cross::Compiler* compiler, const spirv_cross::ID& resourceID)
    {
        return !compiler->get_buffer_block_flags(resourceID).get(spv::DecorationNonWritable);
    }

    inline void WriteConstantBufferDescriptor(const spirv_cross::Compiler* compiler, const spirv_cross::Resource& resource, std::unordered_map<std::string, BufferDescriptor>& buffers, GraphicsBackend backend)
    {
        uint32_t bindPoint = GetSPIRVBindPoint(compiler, resource.id);
        const std::string& bufferName = compiler->get_name(resource.id);

        if (buffers.contains(bufferName))
            return;

        if (backend == GRAPHICS_BACKEND_METAL && bindPoint >= k_MetalConstantBufferBindingOffset)
            bindPoint -= k_MetalConstantBufferBindingOffset;

        const spirv_cross::SPIRType& bufferType = compiler->get_type(resource.base_type_id);
        const uint32_t size = static_cast<uint32_t>(compiler->get_declared_struct_size(bufferType));

        BufferDescriptor bufferDesc{ bindPoint, size, BufferType::CONSTANT_BUFFER, false };

        for (uint32_t i = 0; i < bufferType.member_types.size(); ++i)
        {
            const std::string& varName = compiler->get_member_name(resource.base_type_id, i);
            const uint32_t varOffset = compiler->type_struct_member_offset(bufferType, i);

            bufferDesc.Variables[varName] = varOffset;
        }

        buffers[bufferName] = std::move(bufferDesc);
    }

    //void HandleStructuredBufferReflection(const spirv_cross::Compiler* compiler, const spirv_cross::Resource& resource, std::unordered_map<std::string, BufferDescriptor>& buffers)
    //{
    //    const uint32_t bindPoint = GetSPIRVBindPoint(compiler, resource.id);
    //    const std::string& bufferName = GetSPIRVResourceName(compiler, resource.id);
    //
    //    if (buffers.contains(bufferName))
    //        return;
    //
    //    const spirv_cross::SPIRType &bufferType = compiler->get_type(resource.base_type_id);
    //    const spirv_cross::SPIRType &structType = compiler->get_type(bufferType.member_types[0]);
    //    uint32_t size = structType.basetype == spirv_cross::SPIRType::Struct ? static_cast<uint32_t>(compiler->get_declared_struct_size(structType)) : 0;
    //
    //    BufferDescriptor bufferDesc{bindPoint, size, BufferType::STRUCTURED_BUFFER};
    //
    //    for (uint32_t i = 0; i < structType.member_types.size(); ++i)
    //    {
    //        const std::string& varName = compiler->get_member_name(structType.self, i);
    //        uint32_t varOffset = compiler->type_struct_member_offset(structType, i);
    //
    //        bufferDesc.Variables[varName] = varOffset;
    //    }
    //
    //    buffers[bufferName] = std::move(bufferDesc);
    //}
}

inline void ExtractReflectionFromSPIRV(spirv_cross::Compiler* compiler, Reflection& reflection, GraphicsBackend backend)
{
    spirv_cross::ShaderResources resources = compiler->get_shader_resources();

    for (const spirv_cross::Resource& buffer : resources.uniform_buffers)
        SPIRVReflection_Local::WriteConstantBufferDescriptor(compiler, buffer, reflection.Buffers, backend);

    for (const spirv_cross::Resource& buffer : resources.storage_buffers)
    {
        WriteBufferDescriptor(compiler->get_name(buffer.id), SPIRVReflection_Local::GetSPIRVBindPoint(compiler, buffer.id), 0, BufferType::STRUCTURED_BUFFER, 
            SPIRVReflection_Local::IsSPIRVBufferWritable(compiler, buffer.id), reflection.Buffers);
    }

    for (const spirv_cross::Resource& image : resources.separate_images)
    {
        const spirv_cross::SPIRType& type = compiler->get_type(image.base_type_id);
        if (type.image.dim == spv::DimBuffer)
            WriteBufferDescriptor(compiler->get_name(image.id), SPIRVReflection_Local::GetSPIRVBindPoint(compiler, image.id), 0, BufferType::TYPED_BUFFER, false, reflection.Buffers);
        else
			WriteTextureDescriptor(compiler->get_name(image.id), SPIRVReflection_Local::GetSPIRVBindPoint(compiler, image.id), false, reflection.Textures);
    }

	for (const spirv_cross::Resource& sampler : resources.separate_samplers)
		WriteResourceDescriptor(compiler->get_name(sampler.id), SPIRVReflection_Local::GetSPIRVBindPoint(compiler, sampler.id), reflection.Samplers);

	for (const spirv_cross::Resource& image : resources.sampled_images)
	{
		const uint32_t bindPoint = SPIRVReflection_Local::GetSPIRVBindPoint(compiler, image.id);
		const std::string& imageName = compiler->get_name(image.id);

		WriteTextureDescriptor(imageName, bindPoint, false, reflection.Textures);
		WriteResourceDescriptor("sampler" + imageName, bindPoint, reflection.Samplers);
	}

    for (const spirv_cross::Resource& image : resources.storage_images)
    {
        const spirv_cross::SPIRType& type = compiler->get_type(image.type_id);
        if (type.image.dim == spv::DimBuffer)
	        WriteBufferDescriptor(compiler->get_name(image.id), SPIRVReflection_Local::GetSPIRVBindPoint(compiler, image.id), 0, BufferType::TYPED_BUFFER, true, reflection.Buffers);
        else
            WriteTextureDescriptor(compiler->get_name(image.id), SPIRVReflection_Local::GetSPIRVBindPoint(compiler, image.id), false, reflection.Textures);
    }
}

#endif //RENDER_ENGINE_SHADER_COMPILER_REFLECTION_SPIRV_H
