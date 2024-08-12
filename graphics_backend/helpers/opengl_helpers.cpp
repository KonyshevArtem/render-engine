#include "opengl_helpers.h"

std::string OpenGLHelpers::GetShaderTypeName(ShaderType shaderType)
{
    switch (shaderType)
    {
        case ShaderType::VERTEX_SHADER:
            return "Vertex";
        case ShaderType::FRAGMENT_SHADER:
            return "Fragment";
        case ShaderType::GEOMETRY_SHADER:
            return "Geometry";
        default:
            return "Unknown";
    }
}

GLuint OpenGLHelpers::ToShaderType(ShaderType shaderType)
{
    switch (shaderType)
    {
        case ShaderType::VERTEX_SHADER:
            return GL_VERTEX_SHADER;
        case ShaderType::TESS_CONTROL_SHADER:
            return GL_TESS_CONTROL_SHADER;
        case ShaderType::TESS_EVALUATION_SHADER:
            return GL_TESS_EVALUATION_SHADER;
        case ShaderType::GEOMETRY_SHADER:
            return GL_GEOMETRY_SHADER;
        case ShaderType::FRAGMENT_SHADER:
            return GL_FRAGMENT_SHADER;
    }
}

GLenum OpenGLHelpers::ToBufferBindTarget(BufferBindTarget bindTarget)
{
    switch (bindTarget)
    {
        case BufferBindTarget::ARRAY_BUFFER:
            return GL_ARRAY_BUFFER;
        case BufferBindTarget::COPY_READ_BUFFER:
            return GL_COPY_READ_BUFFER;
        case BufferBindTarget::COPY_WRITE_BUFFER:
            return GL_COPY_WRITE_BUFFER;
        case BufferBindTarget::DRAW_INDIRECT_BUFFER:
            return GL_DRAW_INDIRECT_BUFFER;
        case BufferBindTarget::ELEMENT_ARRAY_BUFFER:
            return GL_ELEMENT_ARRAY_BUFFER;
        case BufferBindTarget::PIXEL_PACK_BUFFER:
            return GL_PIXEL_PACK_BUFFER;
        case BufferBindTarget::PIXEL_UNPACK_BUFFER:
            return GL_PIXEL_UNPACK_BUFFER;
        case BufferBindTarget::TEXTURE_BUFFER:
            return GL_TEXTURE_BUFFER;
        case BufferBindTarget::TRANSFORM_FEEDBACK_BUFFER:
            return GL_TRANSFORM_FEEDBACK_BUFFER;
        case BufferBindTarget::UNIFORM_BUFFER:
            return GL_UNIFORM_BUFFER;
#ifdef GL_ARB_compute_shader
            case BufferBindTarget::DISPATCH_INDIRECT_BUFFER:
                return GL_DISPATCH_INDIRECT_BUFFER;
#endif
#ifdef GL_ARB_query_buffer_object
            case BufferBindTarget::QUERY_BUFFER:
                return GL_QUERY_BUFFER;
#endif
#ifdef GL_ARB_shader_storage_buffer_object
            case BufferBindTarget::SHADER_STORAGE_BUFFER:
                return GL_SHADER_STORAGE_BUFFER;
#endif
        default:
            return 0;
    }
}

GLenum OpenGLHelpers::ToBufferUsageHint(BufferUsageHint usageHint)
{
    switch (usageHint)
    {
        case BufferUsageHint::STREAM_DRAW:
            return GL_STREAM_DRAW;
        case BufferUsageHint::STREAM_READ:
            return GL_STREAM_READ;
        case BufferUsageHint::STREAM_COPY:
            return GL_STREAM_COPY;
        case BufferUsageHint::STATIC_DRAW:
            return GL_STATIC_DRAW;
        case BufferUsageHint::STATIC_READ:
            return GL_STATIC_READ;
        case BufferUsageHint::STATIC_COPY:
            return GL_STATIC_COPY;
        case BufferUsageHint::DYNAMIC_DRAW:
            return GL_DYNAMIC_DRAW;
        case BufferUsageHint::DYNAMIC_READ:
            return GL_DYNAMIC_READ;
        case BufferUsageHint::DYNAMIC_COPY:
            return GL_DYNAMIC_COPY;
        default:
            return 0;
    }
}

GLenum OpenGLHelpers::ToPrimitiveType(PrimitiveType primitiveType)
{
    switch (primitiveType)
    {
        case PrimitiveType::POINTS:
            return GL_POINTS;
        case PrimitiveType::LINE_STRIP:
            return GL_LINE_STRIP;
        case PrimitiveType::LINE_LOOP:
            return GL_LINE_LOOP;
        case PrimitiveType::LINES:
            return GL_LINES;
        case PrimitiveType::LINE_STRIP_ADJACENCY:
            return GL_LINE_STRIP_ADJACENCY;
        case PrimitiveType::LINES_ADJACENCY:
            return GL_LINES_ADJACENCY;
        case PrimitiveType::TRIANGLE_STRIP:
            return GL_TRIANGLE_STRIP;
        case PrimitiveType::TRIANGLE_FAN:
            return GL_TRIANGLE_FAN;
        case PrimitiveType::TRIANGLES:
            return GL_TRIANGLES;
        case PrimitiveType::TRIANGLE_STRIP_ADJACENCY:
            return GL_TRIANGLE_STRIP_ADJACENCY;
        case PrimitiveType::TRIANGLES_ADJACENCY:
            return GL_TRIANGLES_ADJACENCY;
        case PrimitiveType::PATCHES:
            return GL_PATCHES;
    }
}

GLenum OpenGLHelpers::ToIndicesDataType(IndicesDataType dataType)
{
    switch (dataType)
    {
        case IndicesDataType::UNSIGNED_BYTE:
            return GL_UNSIGNED_BYTE;
        case IndicesDataType::UNSIGNED_SHORT:
            return GL_UNSIGNED_SHORT;
        case IndicesDataType::UNSIGNED_INT:
            return GL_UNSIGNED_INT;
    }
}

GLenum OpenGLHelpers::ToVertexAttributeDataType(VertexAttributeDataType dataType)
{
    switch (dataType)
    {
        case VertexAttributeDataType::BYTE:
            return GL_BYTE;
        case VertexAttributeDataType::UNSIGNED_BYTE:
            return GL_UNSIGNED_BYTE;
        case VertexAttributeDataType::SHORT:
            return GL_SHORT;
        case VertexAttributeDataType::UNSIGNED_SHORT:
            return GL_UNSIGNED_SHORT;
        case VertexAttributeDataType::INT:
            return GL_INT;
        case VertexAttributeDataType::UNSIGNED_INT:
            return GL_UNSIGNED_INT;
        case VertexAttributeDataType::HALF_FLOAT:
            return GL_HALF_FLOAT;
        case VertexAttributeDataType::FLOAT:
            return GL_FLOAT;
        case VertexAttributeDataType::DOUBLE:
            return GL_DOUBLE;
        case VertexAttributeDataType::FIXED:
            return GL_FIXED;
        case VertexAttributeDataType::INT_2_10_10_10_REV:
            return GL_INT_2_10_10_10_REV;
        case VertexAttributeDataType::UNSIGNED_INT_2_10_10_10_REV:
            return GL_UNSIGNED_INT_2_10_10_10_REV;
        case VertexAttributeDataType::UNSIGNED_INT_10F_11F_11F_REV:
            return GL_UNSIGNED_INT_10F_11F_11F_REV;
    }
}

GLenum OpenGLHelpers::ToTextureType(TextureType textureType)
{
    switch (textureType)
    {
        case TextureType::TEXTURE_1D:
            return GL_TEXTURE_1D;
        case TextureType::TEXTURE_1D_ARRAY:
            return GL_TEXTURE_1D_ARRAY;
        case TextureType::TEXTURE_2D:
            return GL_TEXTURE_2D;
        case TextureType::TEXTURE_2D_MULTISAMPLE:
            return GL_TEXTURE_2D_MULTISAMPLE;
        case TextureType::TEXTURE_2D_ARRAY:
            return GL_TEXTURE_2D_ARRAY;
        case TextureType::TEXTURE_2D_MULTISAMPLE_ARRAY:
            return GL_TEXTURE_2D_MULTISAMPLE_ARRAY;
        case TextureType::TEXTURE_3D:
            return GL_TEXTURE_3D;
        case TextureType::TEXTURE_CUBEMAP:
            return GL_TEXTURE_CUBE_MAP;
        case TextureType::TEXTURE_CUBEMAP_ARRAY:
            return GL_TEXTURE_CUBE_MAP_ARRAY;
        case TextureType::TEXTURE_RECTANGLE:
            return GL_TEXTURE_RECTANGLE;
        case TextureType::TEXTURE_BUFFER:
            return GL_TEXTURE_BUFFER;
    }
}

GLenum OpenGLHelpers::ToTextureInternalFormat(TextureInternalFormat format)
{
    switch (format)
    {
        case TextureInternalFormat::RED:
            return GL_RED;
        case TextureInternalFormat::RG:
            return GL_RG;
        case TextureInternalFormat::RGB:
            return GL_RGB;
        case TextureInternalFormat::RGBA:
            return GL_RGBA;
        case TextureInternalFormat::SRGB:
            return GL_SRGB;
        case TextureInternalFormat::SRGB_ALPHA:
            return GL_SRGB_ALPHA;
        case TextureInternalFormat::COMPRESSED_RGB:
            return GL_COMPRESSED_RGB;
        case TextureInternalFormat::COMPRESSED_RGBA:
            return GL_COMPRESSED_RGBA;
        case TextureInternalFormat::COMPRESSED_SRGB:
            return GL_COMPRESSED_SRGB;
        case TextureInternalFormat::COMPRESSED_SRGB_ALPHA:
            return GL_COMPRESSED_SRGB_ALPHA;
#if GL_ARB_texture_rg
        case TextureInternalFormat::COMPRESSED_RED:
            return GL_COMPRESSED_RED;
        case TextureInternalFormat::COMPRESSED_RG:
            return GL_COMPRESSED_RG;
#endif
        case TextureInternalFormat::R8:
            return GL_R8;
        case TextureInternalFormat::R8_SNORM:
            return GL_R8_SNORM;
        case TextureInternalFormat::R16:
            return GL_R16;
        case TextureInternalFormat::R16_SNORM:
            return GL_R16_SNORM;
        case TextureInternalFormat::RG8:
            return GL_RG8;
        case TextureInternalFormat::RG8_SNORM:
            return GL_RG8_SNORM;
        case TextureInternalFormat::RG16:
            return GL_RG16;
        case TextureInternalFormat::RG16_SNORM:
            return GL_RG16_SNORM;
        case TextureInternalFormat::R3_G3_B2:
            return GL_R3_G3_B2;
        case TextureInternalFormat::RGB4:
            return GL_RGB4;
        case TextureInternalFormat::RGB5:
            return GL_RGB5;
        case TextureInternalFormat::RGB8:
            return GL_RGB8;
        case TextureInternalFormat::RGB8_SNORM:
            return GL_RGB8_SNORM;
        case TextureInternalFormat::RGB10:
            return GL_RGB10;
        case TextureInternalFormat::RGB12:
            return GL_RGB12;
        case TextureInternalFormat::RGB16:
            return GL_RGB16;
        case TextureInternalFormat::RGBA2:
            return GL_RGBA2;
        case TextureInternalFormat::RGBA4:
            return GL_RGBA4;
        case TextureInternalFormat::RGB5_A1:
            return GL_RGB5_A1;
        case TextureInternalFormat::RGBA8:
            return GL_RGBA8;
        case TextureInternalFormat::RGBA8_SNORM:
            return GL_RGBA8_SNORM;
        case TextureInternalFormat::RGB10_A2:
            return GL_RGB10_A2;
        case TextureInternalFormat::RGB10_A2UI:
            return GL_RGB10_A2UI;
        case TextureInternalFormat::RGBA12:
            return GL_RGBA12;
        case TextureInternalFormat::RGBA16:
            return GL_RGBA16;
        case TextureInternalFormat::SRGB8:
            return GL_SRGB8;
        case TextureInternalFormat::SRGB8_ALPHA8:
            return GL_SRGB8_ALPHA8;
        case TextureInternalFormat::R16F:
            return GL_R16F;
        case TextureInternalFormat::RG16F:
            return GL_RG16F;
        case TextureInternalFormat::RGB16F:
            return GL_RGB16F;
        case TextureInternalFormat::RGBA16F:
            return GL_RGBA16F;
        case TextureInternalFormat::R32F:
            return GL_R32F;
        case TextureInternalFormat::RG32F:
            return GL_RG32F;
        case TextureInternalFormat::RGB32F:
            return GL_RGB32F;
        case TextureInternalFormat::RGBA32F:
            return GL_RGBA32F;
        case TextureInternalFormat::R11F_G11F_B10F:
            return GL_R11F_G11F_B10F;
        case TextureInternalFormat::RGB9_E5:
            return GL_RGB9_E5;
        case TextureInternalFormat::R8I:
            return GL_R8I;
        case TextureInternalFormat::R8UI:
            return GL_R8UI;
        case TextureInternalFormat::R16I:
            return GL_R16I;
        case TextureInternalFormat::R16UI:
            return GL_R16UI;
        case TextureInternalFormat::R32I:
            return GL_R32I;
        case TextureInternalFormat::R32UI:
            return GL_R32UI;
        case TextureInternalFormat::RG8I:
            return GL_RG8I;
        case TextureInternalFormat::RG8UI:
            return GL_RG8UI;
        case TextureInternalFormat::RG16I:
            return GL_RG16I;
        case TextureInternalFormat::RG16UI:
            return GL_RG16UI;
        case TextureInternalFormat::RG32I:
            return GL_RG32I;
        case TextureInternalFormat::RG32UI:
            return GL_RG32UI;
        case TextureInternalFormat::RGB8I:
            return GL_RGB8I;
        case TextureInternalFormat::RGB8UI:
            return GL_RGB8UI;
        case TextureInternalFormat::RGB16I:
            return GL_RGB16I;
        case TextureInternalFormat::RGB16UI:
            return GL_RGB16UI;
        case TextureInternalFormat::RGB32I:
            return GL_RGB32I;
        case TextureInternalFormat::RGB32UI:
            return GL_RGB32UI;
        case TextureInternalFormat::RGBA8I:
            return GL_RGBA8I;
        case TextureInternalFormat::RGBA8UI:
            return GL_RGBA8UI;
        case TextureInternalFormat::RGBA16I:
            return GL_RGBA16I;
        case TextureInternalFormat::RGBA16UI:
            return GL_RGBA16UI;
        case TextureInternalFormat::RGBA32I:
            return GL_RGBA32I;
        case TextureInternalFormat::RGBA32UI:
            return GL_RGBA32UI;
#if GL_EXT_texture_compression_s3tc
        case TextureInternalFormat::COMPRESSED_RGB_S3TC_DXT1_EXT:
            return GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
        case TextureInternalFormat::COMPRESSED_RGBA_S3TC_DXT1_EXT:
            return GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
        case TextureInternalFormat::COMPRESSED_RGBA_S3TC_DXT3_EXT:
            return GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
        case TextureInternalFormat::COMPRESSED_RGBA_S3TC_DXT5_EXT:
            return GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
#endif
#if GL_EXT_texture_sRGB
        case TextureInternalFormat::COMPRESSED_SRGB_S3TC_DXT1_EXT:
            return GL_COMPRESSED_SRGB_S3TC_DXT1_EXT;
        case TextureInternalFormat::COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT:
            return GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT;
        case TextureInternalFormat::COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT:
            return GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT;
        case TextureInternalFormat::COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT:
            return GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT;
#endif
#if GL_ARB_texture_compression_rgtc
        case TextureInternalFormat::COMPRESSED_RED_RGTC1:
            return GL_COMPRESSED_RED_RGTC1;
        case TextureInternalFormat::COMPRESSED_SIGNED_RED_RGTC1:
            return GL_COMPRESSED_SIGNED_RED_RGTC1;
        case TextureInternalFormat::COMPRESSED_RG_RGTC2:
            return GL_COMPRESSED_RG_RGTC2;
        case TextureInternalFormat::COMPRESSED_SIGNED_RG_RGTC2:
            return GL_COMPRESSED_SIGNED_RG_RGTC2;
#endif
        case TextureInternalFormat::DEPTH_COMPONENT:
            return GL_DEPTH_COMPONENT;
        case TextureInternalFormat::DEPTH_STENCIL:
            return GL_DEPTH_STENCIL;
        default:
            return 0;
    }
}

TextureInternalFormat OpenGLHelpers::FromTextureInternalFormat(GLenum format)
{
    switch (format)
    {
        case GL_RED:
            return TextureInternalFormat::RED;
        case GL_RG:
            return TextureInternalFormat::RG;
        case GL_RGB:
            return TextureInternalFormat::RGB;
        case GL_RGBA:
            return TextureInternalFormat::RGBA;
        case GL_SRGB:
            return TextureInternalFormat::SRGB;
        case GL_SRGB_ALPHA:
            return TextureInternalFormat::SRGB_ALPHA;
        case GL_COMPRESSED_RGB:
            return TextureInternalFormat::COMPRESSED_RGB;
        case GL_COMPRESSED_RGBA:
            return TextureInternalFormat::COMPRESSED_RGBA;
        case GL_COMPRESSED_SRGB:
            return TextureInternalFormat::COMPRESSED_SRGB;
        case GL_COMPRESSED_SRGB_ALPHA:
            return TextureInternalFormat::COMPRESSED_SRGB_ALPHA;
#if GL_ARB_texture_rg
        case GL_COMPRESSED_RED:
            return TextureInternalFormat::COMPRESSED_RED;
        case GL_COMPRESSED_RG:
            return TextureInternalFormat::COMPRESSED_RG;
#endif
        case GL_R8:
            return TextureInternalFormat::R8;
        case GL_R8_SNORM:
            return TextureInternalFormat::R8_SNORM;
        case GL_R16:
            return TextureInternalFormat::R16;
        case GL_R16_SNORM:
            return TextureInternalFormat::R16_SNORM;
        case GL_RG8:
            return TextureInternalFormat::RG8;
        case GL_RG8_SNORM:
            return TextureInternalFormat::RG8_SNORM;
        case GL_RG16:
            return TextureInternalFormat::RG16;
        case GL_RG16_SNORM:
            return TextureInternalFormat::RG16_SNORM;
        case GL_R3_G3_B2:
            return TextureInternalFormat::R3_G3_B2;
        case GL_RGB4:
            return TextureInternalFormat::RGB4;
        case GL_RGB5:
            return TextureInternalFormat::RGB5;
        case GL_RGB8:
            return TextureInternalFormat::RGB8;
        case GL_RGB8_SNORM:
            return TextureInternalFormat::RGB8_SNORM;
        case GL_RGB10:
            return TextureInternalFormat::RGB10;
        case GL_RGB12:
            return TextureInternalFormat::RGB12;
        case GL_RGB16:
            return TextureInternalFormat::RGB16;
        case GL_RGBA2:
            return TextureInternalFormat::RGBA2;
        case GL_RGBA4:
            return TextureInternalFormat::RGBA4;
        case GL_RGB5_A1:
            return TextureInternalFormat::RGB5_A1;
        case GL_RGBA8:
            return TextureInternalFormat::RGBA8;
        case GL_RGBA8_SNORM:
            return TextureInternalFormat::RGBA8_SNORM;
        case GL_RGB10_A2:
            return TextureInternalFormat::RGB10_A2;
        case GL_RGB10_A2UI:
            return TextureInternalFormat::RGB10_A2UI;
        case GL_RGBA12:
            return TextureInternalFormat::RGBA12;
        case GL_RGBA16:
            return TextureInternalFormat::RGBA16;
        case GL_SRGB8:
            return TextureInternalFormat::SRGB8;
        case GL_SRGB8_ALPHA8:
            return TextureInternalFormat::SRGB8_ALPHA8;
        case GL_R16F:
            return TextureInternalFormat::R16F;
        case GL_RG16F:
            return TextureInternalFormat::RG16F;
        case GL_RGB16F:
            return TextureInternalFormat::RGB16F;
        case GL_RGBA16F:
            return TextureInternalFormat::RGBA16F;
        case GL_R32F:
            return TextureInternalFormat::R32F;
        case GL_RG32F:
            return TextureInternalFormat::RG32F;
        case GL_RGB32F:
            return TextureInternalFormat::RGB32F;
        case GL_RGBA32F:
            return TextureInternalFormat::RGBA32F;
        case GL_R11F_G11F_B10F:
            return TextureInternalFormat::R11F_G11F_B10F;
        case GL_RGB9_E5:
            return TextureInternalFormat::RGB9_E5;
        case GL_R8I:
            return TextureInternalFormat::R8I;
        case GL_R8UI:
            return TextureInternalFormat::R8UI;
        case GL_R16I:
            return TextureInternalFormat::R16I;
        case GL_R16UI:
            return TextureInternalFormat::R16UI;
        case GL_R32I:
            return TextureInternalFormat::R32I;
        case GL_R32UI:
            return TextureInternalFormat::R32UI;
        case GL_RG8I:
            return TextureInternalFormat::RG8I;
        case GL_RG8UI:
            return TextureInternalFormat::RG8UI;
        case GL_RG16I:
            return TextureInternalFormat::RG16I;
        case GL_RG16UI:
            return TextureInternalFormat::RG16UI;
        case GL_RG32I:
            return TextureInternalFormat::RG32I;
        case GL_RG32UI:
            return TextureInternalFormat::RG32UI;
        case GL_RGB8I:
            return TextureInternalFormat::RGB8I;
        case GL_RGB8UI:
            return TextureInternalFormat::RGB8UI;
        case GL_RGB16I:
            return TextureInternalFormat::RGB16I;
        case GL_RGB16UI:
            return TextureInternalFormat::RGB16UI;
        case GL_RGB32I:
            return TextureInternalFormat::RGB32I;
        case GL_RGB32UI:
            return TextureInternalFormat::RGB32UI;
        case GL_RGBA8I:
            return TextureInternalFormat::RGBA8I;
        case GL_RGBA8UI:
            return TextureInternalFormat::RGBA8UI;
        case GL_RGBA16I:
            return TextureInternalFormat::RGBA16I;
        case GL_RGBA16UI:
            return TextureInternalFormat::RGBA16UI;
        case GL_RGBA32I:
            return TextureInternalFormat::RGBA32I;
        case GL_RGBA32UI:
            return TextureInternalFormat::RGBA32UI;
#if GL_EXT_texture_compression_s3tc
        case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
            return TextureInternalFormat::COMPRESSED_RGB_S3TC_DXT1_EXT;
        case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
            return TextureInternalFormat::COMPRESSED_RGBA_S3TC_DXT1_EXT;
        case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
            return TextureInternalFormat::COMPRESSED_RGBA_S3TC_DXT3_EXT;
        case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
            return TextureInternalFormat::COMPRESSED_RGBA_S3TC_DXT5_EXT;
#endif
#if GL_EXT_texture_sRGB
        case GL_COMPRESSED_SRGB_S3TC_DXT1_EXT:
            return TextureInternalFormat::COMPRESSED_SRGB_S3TC_DXT1_EXT;
        case GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT:
            return TextureInternalFormat::COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT;
        case GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT:
            return TextureInternalFormat::COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT;
        case GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT:
            return TextureInternalFormat::COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT;
#endif
#if GL_ARB_texture_compression_rgtc
        case GL_COMPRESSED_RED_RGTC1:
            return TextureInternalFormat::COMPRESSED_RED_RGTC1;
        case GL_COMPRESSED_SIGNED_RED_RGTC1:
            return TextureInternalFormat::COMPRESSED_SIGNED_RED_RGTC1;
        case GL_COMPRESSED_RG_RGTC2:
            return TextureInternalFormat::COMPRESSED_RG_RGTC2;
        case GL_COMPRESSED_SIGNED_RG_RGTC2:
            return TextureInternalFormat::COMPRESSED_SIGNED_RG_RGTC2;
#endif
        case GL_DEPTH_COMPONENT:
            return TextureInternalFormat::DEPTH_COMPONENT;
        case GL_DEPTH_STENCIL:
            return TextureInternalFormat::DEPTH_STENCIL;
        default:
            return TextureInternalFormat::RGBA8;
    }
}

GLenum OpenGLHelpers::ToTextureFormat(TextureInternalFormat format)
{
    switch (format)
    {
        case TextureInternalFormat::RED:
        case TextureInternalFormat::COMPRESSED_RED:
        case TextureInternalFormat::COMPRESSED_RG:
        case TextureInternalFormat::R8:
        case TextureInternalFormat::R8_SNORM:
        case TextureInternalFormat::R16:
        case TextureInternalFormat::R16_SNORM:
        case TextureInternalFormat::R16F:
        case TextureInternalFormat::R32F:
        case TextureInternalFormat::R8I:
        case TextureInternalFormat::R8UI:
        case TextureInternalFormat::R16I:
        case TextureInternalFormat::R16UI:
        case TextureInternalFormat::R32I:
        case TextureInternalFormat::R32UI:
        case TextureInternalFormat::COMPRESSED_RED_RGTC1:
        case TextureInternalFormat::COMPRESSED_SIGNED_RED_RGTC1:
            return GL_RED;
        case TextureInternalFormat::RG:
        case TextureInternalFormat::RG8:
        case TextureInternalFormat::RG8_SNORM:
        case TextureInternalFormat::RG16:
        case TextureInternalFormat::RG16_SNORM:
        case TextureInternalFormat::RG16F:
        case TextureInternalFormat::RG32F:
        case TextureInternalFormat::RG8I:
        case TextureInternalFormat::RG8UI:
        case TextureInternalFormat::RG16I:
        case TextureInternalFormat::RG16UI:
        case TextureInternalFormat::RG32I:
        case TextureInternalFormat::RG32UI:
        case TextureInternalFormat::COMPRESSED_RG_RGTC2:
        case TextureInternalFormat::COMPRESSED_SIGNED_RG_RGTC2:
            return GL_RG;
        case TextureInternalFormat::RGB:
        case TextureInternalFormat::SRGB:
        case TextureInternalFormat::COMPRESSED_RGB:
        case TextureInternalFormat::COMPRESSED_SRGB:
        case TextureInternalFormat::R3_G3_B2:
        case TextureInternalFormat::RGB4:
        case TextureInternalFormat::RGB5:
        case TextureInternalFormat::RGB8:
        case TextureInternalFormat::RGB8_SNORM:
        case TextureInternalFormat::RGB10:
        case TextureInternalFormat::RGB12:
        case TextureInternalFormat::RGB16:
        case TextureInternalFormat::SRGB8:
        case TextureInternalFormat::RGB16F:
        case TextureInternalFormat::RGB32F:
        case TextureInternalFormat::R11F_G11F_B10F:
        case TextureInternalFormat::RGB8I:
        case TextureInternalFormat::RGB8UI:
        case TextureInternalFormat::RGB16I:
        case TextureInternalFormat::RGB16UI:
        case TextureInternalFormat::RGB32I:
        case TextureInternalFormat::RGB32UI:
        case TextureInternalFormat::COMPRESSED_RGB_S3TC_DXT1_EXT:
        case TextureInternalFormat::COMPRESSED_SRGB_S3TC_DXT1_EXT:
            return GL_RGB;
        case TextureInternalFormat::RGBA:
        case TextureInternalFormat::SRGB_ALPHA:
        case TextureInternalFormat::COMPRESSED_RGBA:
        case TextureInternalFormat::COMPRESSED_SRGB_ALPHA:
        case TextureInternalFormat::RGBA2:
        case TextureInternalFormat::RGBA4:
        case TextureInternalFormat::RGB5_A1:
        case TextureInternalFormat::RGBA8:
        case TextureInternalFormat::RGBA8_SNORM:
        case TextureInternalFormat::SRGB8_ALPHA8:
        case TextureInternalFormat::RGB10_A2:
        case TextureInternalFormat::RGB10_A2UI:
        case TextureInternalFormat::RGBA12:
        case TextureInternalFormat::RGBA16:
        case TextureInternalFormat::RGBA16F:
        case TextureInternalFormat::RGBA32F:
        case TextureInternalFormat::RGB9_E5:
        case TextureInternalFormat::RGBA8I:
        case TextureInternalFormat::RGBA8UI:
        case TextureInternalFormat::RGBA16I:
        case TextureInternalFormat::RGBA16UI:
        case TextureInternalFormat::RGBA32I:
        case TextureInternalFormat::RGBA32UI:
        case TextureInternalFormat::COMPRESSED_RGBA_S3TC_DXT1_EXT:
        case TextureInternalFormat::COMPRESSED_RGBA_S3TC_DXT3_EXT:
        case TextureInternalFormat::COMPRESSED_RGBA_S3TC_DXT5_EXT:
        case TextureInternalFormat::COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT:
        case TextureInternalFormat::COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT:
        case TextureInternalFormat::COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT:
            return GL_RGBA;
        case TextureInternalFormat::DEPTH_COMPONENT:
        case TextureInternalFormat::DEPTH_STENCIL:
            return GL_DEPTH_COMPONENT;
    }
}

GLenum OpenGLHelpers::ToTextureDataType(TextureInternalFormat format)
{
    switch (format)
    {
        case TextureInternalFormat::DEPTH_COMPONENT:
            return GL_FLOAT;
        default:
            return GL_UNSIGNED_BYTE;
    }
}

GLenum OpenGLHelpers::ToTextureUnit(int binding)
{
    return GL_TEXTURE0 + binding;
}

GLint OpenGLHelpers::ToTextureWrapMode(TextureWrapMode wrapMode)
{
    switch (wrapMode)
    {
        case TextureWrapMode::CLAMP_TO_EDGE:
            return GL_CLAMP_TO_EDGE;
        case TextureWrapMode::CLAMP_TO_BORDER:
            return GL_CLAMP_TO_BORDER;
        case TextureWrapMode::MIRRORED_REPEAT:
            return GL_MIRRORED_REPEAT;
        case TextureWrapMode::REPEAT:
            return GL_REPEAT;
    }
}

GLenum OpenGLHelpers::ToTextureTarget(TextureType type, int slice)
{
    switch (type)
    {
        case TextureType::TEXTURE_1D:
            return GL_TEXTURE_1D;
        case TextureType::TEXTURE_1D_ARRAY:
            return GL_TEXTURE_1D_ARRAY;
        case TextureType::TEXTURE_2D:
            return GL_TEXTURE_2D;
        case TextureType::TEXTURE_2D_MULTISAMPLE:
            return GL_TEXTURE_2D_MULTISAMPLE;
        case TextureType::TEXTURE_2D_ARRAY:
            return GL_TEXTURE_2D_ARRAY;
        case TextureType::TEXTURE_2D_MULTISAMPLE_ARRAY:
            return GL_TEXTURE_2D_MULTISAMPLE_ARRAY;
        case TextureType::TEXTURE_3D:
            return GL_TEXTURE_3D;
        case TextureType::TEXTURE_CUBEMAP:
        case TextureType::TEXTURE_CUBEMAP_ARRAY:
            return GL_TEXTURE_CUBE_MAP_POSITIVE_X + slice;
        case TextureType::TEXTURE_RECTANGLE:
            return GL_TEXTURE_RECTANGLE;
        case TextureType::TEXTURE_BUFFER:
            return GL_TEXTURE_BUFFER;
        default:
            return 0;
    }
}

void OpenGLHelpers::ToTextureFilteringMode(TextureFilteringMode filteringMode, GLint &outMinFilter, GLint &outMagFilter)
{
    switch (filteringMode)
    {
        case TextureFilteringMode::NEAREST:
        {
            outMinFilter = GL_NEAREST;
            outMagFilter = GL_NEAREST;
            break;
        }
        case TextureFilteringMode::LINEAR:
        {
            outMinFilter = GL_LINEAR;
            outMagFilter = GL_LINEAR;
            break;
        }
        case TextureFilteringMode::NEAREST_MIPMAP_NEAREST:
        {
            outMinFilter = GL_NEAREST_MIPMAP_NEAREST;
            outMagFilter = GL_NEAREST;
            break;
        }
        case TextureFilteringMode::LINEAR_MIPMAP_NEAREST:
        {
            outMinFilter = GL_LINEAR_MIPMAP_NEAREST;
            outMagFilter = GL_LINEAR;
            break;
        }
        case TextureFilteringMode::NEAREST_MIPMAP_LINEAR:
        {
            outMinFilter = GL_NEAREST_MIPMAP_LINEAR;
            outMagFilter = GL_NEAREST;
            break;
        }
        case TextureFilteringMode::LINEAR_MIPMAP_LINEAR:
        {
            outMinFilter = GL_LINEAR_MIPMAP_LINEAR;
            outMagFilter = GL_LINEAR;
            break;
        }
    }
}

GLenum OpenGLHelpers::ToFramebufferAttachment(FramebufferAttachment attachment)
{
    switch (attachment)
    {
        case FramebufferAttachment::COLOR_ATTACHMENT0:
            return GL_COLOR_ATTACHMENT0;
        case FramebufferAttachment::COLOR_ATTACHMENT1:
            return GL_COLOR_ATTACHMENT1;
        case FramebufferAttachment::COLOR_ATTACHMENT2:
            return GL_COLOR_ATTACHMENT2;
        case FramebufferAttachment::COLOR_ATTACHMENT3:
            return GL_COLOR_ATTACHMENT3;
        case FramebufferAttachment::COLOR_ATTACHMENT4:
            return GL_COLOR_ATTACHMENT4;
        case FramebufferAttachment::COLOR_ATTACHMENT5:
            return GL_COLOR_ATTACHMENT5;
        case FramebufferAttachment::COLOR_ATTACHMENT6:
            return GL_COLOR_ATTACHMENT6;
        case FramebufferAttachment::COLOR_ATTACHMENT7:
            return GL_COLOR_ATTACHMENT7;
        case FramebufferAttachment::COLOR_ATTACHMENT8:
            return GL_COLOR_ATTACHMENT8;
        case FramebufferAttachment::COLOR_ATTACHMENT9:
            return GL_COLOR_ATTACHMENT9;
        case FramebufferAttachment::COLOR_ATTACHMENT10:
            return GL_COLOR_ATTACHMENT10;
        case FramebufferAttachment::COLOR_ATTACHMENT11:
            return GL_COLOR_ATTACHMENT11;
        case FramebufferAttachment::COLOR_ATTACHMENT12:
            return GL_COLOR_ATTACHMENT12;
        case FramebufferAttachment::COLOR_ATTACHMENT13:
            return GL_COLOR_ATTACHMENT13;
        case FramebufferAttachment::COLOR_ATTACHMENT14:
            return GL_COLOR_ATTACHMENT14;
        case FramebufferAttachment::COLOR_ATTACHMENT15:
            return GL_COLOR_ATTACHMENT15;
        case FramebufferAttachment::DEPTH_ATTACHMENT:
            return GL_DEPTH_ATTACHMENT;
        case FramebufferAttachment::STENCIL_ATTACHMENT:
            return GL_STENCIL_ATTACHMENT;
        case FramebufferAttachment::DEPTH_STENCIL_ATTACHMENT:
            return GL_DEPTH_STENCIL_ATTACHMENT;
        case FramebufferAttachment::MAX:
            return -1;
    }
}

GLenum OpenGLHelpers::ToDepthCompareFunction(DepthFunction function)
{
    switch (function)
    {
        case DepthFunction::NEVER:
            return GL_NEVER;
        case DepthFunction::LESS:
            return GL_LESS;
        case DepthFunction::EQUAL:
            return GL_EQUAL;
        case DepthFunction::LEQUAL:
            return GL_LEQUAL;
        case DepthFunction::GREATER:
            return GL_GREATER;
        case DepthFunction::NOTEQUAL:
            return GL_NOTEQUAL;
        case DepthFunction::GEQUAL:
            return GL_GEQUAL;
        case DepthFunction::ALWAYS:
            return GL_ALWAYS;
    }
}

GLenum OpenGLHelpers::ToCullFaceOrientation(CullFaceOrientation orientation)
{
    return orientation == CullFaceOrientation::CLOCKWISE ? GL_CW : GL_CCW;
}

GLenum OpenGLHelpers::ToCullFace(CullFace face)
{
    switch (face)
    {
        case CullFace::NONE:
            return 0;
        case CullFace::FRONT:
            return GL_FRONT;
        case CullFace::BACK:
            return GL_BACK;
    }
}
