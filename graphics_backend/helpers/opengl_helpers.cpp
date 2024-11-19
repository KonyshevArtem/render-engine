#ifdef RENDER_BACKEND_OPENGL

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

GLenum OpenGLHelpers::ToTextureInternalFormat(TextureInternalFormat format, bool isLinear)
{
    switch (format)
    {
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
            return isLinear ? GL_RGB8 : GL_SRGB8;
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
            return isLinear ? GL_RGBA8 : GL_SRGB8_ALPHA8;
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
        case TextureInternalFormat::BGRA8:
        case TextureInternalFormat::BGRA8_SNORM:
            return GL_BGRA;
#if GL_EXT_texture_compression_s3tc && GL_EXT_texture_sRGB
        case TextureInternalFormat::BC1_RGB:
            return isLinear ? GL_COMPRESSED_RGB_S3TC_DXT1_EXT : GL_COMPRESSED_SRGB_S3TC_DXT1_EXT;
        case TextureInternalFormat::BC1_RGBA:
            return isLinear ? GL_COMPRESSED_RGBA_S3TC_DXT1_EXT : GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT;
        case TextureInternalFormat::BC2:
            return isLinear ? GL_COMPRESSED_RGBA_S3TC_DXT3_EXT : GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT;
        case TextureInternalFormat::BC3:
            return isLinear ? GL_COMPRESSED_RGBA_S3TC_DXT5_EXT : GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT;
#endif
        case TextureInternalFormat::BC4:
            return GL_COMPRESSED_RED_RGTC1;
        case TextureInternalFormat::BC5:
            return GL_COMPRESSED_RG_RGTC2;
        case TextureInternalFormat::BC6H:
            return GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT;
        case TextureInternalFormat::BC7:
            return isLinear ? GL_COMPRESSED_RGBA_BPTC_UNORM : GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM;
        case TextureInternalFormat::DEPTH_COMPONENT:
            return GL_DEPTH_COMPONENT;
        case TextureInternalFormat::DEPTH_STENCIL:
            return GL_DEPTH_STENCIL;
        default:
            return 0;
    }
}

TextureInternalFormat OpenGLHelpers::FromTextureInternalFormat(GLenum format, bool& outIsLinear)
{
    outIsLinear = !(format == GL_SRGB8 |
                    format == GL_SRGB8_ALPHA8 |
                    format == GL_COMPRESSED_SRGB_S3TC_DXT1_EXT |
                    format == GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT |
                    format == GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT |
                    format == GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT |
                    format == GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM);

    switch (format)
    {
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
        case GL_SRGB8:
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
        case GL_SRGB8_ALPHA8:
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
        case GL_BGRA:
            return TextureInternalFormat::BGRA8;
#if GL_EXT_texture_compression_s3tc && GL_EXT_texture_sRGB
        case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
        case GL_COMPRESSED_SRGB_S3TC_DXT1_EXT:
            return TextureInternalFormat::BC1_RGB;
        case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
        case GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT:
            return TextureInternalFormat::BC1_RGBA;
        case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
        case GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT:
            return TextureInternalFormat::BC2;
        case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
        case GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT:
            return TextureInternalFormat::BC3;
#endif
        case GL_COMPRESSED_RED_RGTC1:
            return TextureInternalFormat::BC4;
        case GL_COMPRESSED_RG_RGTC2:
            return TextureInternalFormat::BC5;
        case GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT:
            return TextureInternalFormat::BC6H;
        case GL_COMPRESSED_RGBA_BPTC_UNORM:
        case GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM:
            return TextureInternalFormat::BC7;
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
        case TextureInternalFormat::BC4:
            return GL_RED;
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
        case TextureInternalFormat::BC5:
            return GL_RG;
        case TextureInternalFormat::R3_G3_B2:
        case TextureInternalFormat::RGB4:
        case TextureInternalFormat::RGB5:
        case TextureInternalFormat::RGB8:
        case TextureInternalFormat::RGB8_SNORM:
        case TextureInternalFormat::RGB10:
        case TextureInternalFormat::RGB12:
        case TextureInternalFormat::RGB16:
        case TextureInternalFormat::RGB16F:
        case TextureInternalFormat::RGB32F:
        case TextureInternalFormat::R11F_G11F_B10F:
        case TextureInternalFormat::RGB8I:
        case TextureInternalFormat::RGB8UI:
        case TextureInternalFormat::RGB16I:
        case TextureInternalFormat::RGB16UI:
        case TextureInternalFormat::RGB32I:
        case TextureInternalFormat::RGB32UI:
        case TextureInternalFormat::BC1_RGB:
        case TextureInternalFormat::BC6H:
            return GL_RGB;
        case TextureInternalFormat::RGBA2:
        case TextureInternalFormat::RGBA4:
        case TextureInternalFormat::RGB5_A1:
        case TextureInternalFormat::RGBA8:
        case TextureInternalFormat::RGBA8_SNORM:
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
        case TextureInternalFormat::BC1_RGBA:
        case TextureInternalFormat::BC2:
        case TextureInternalFormat::BC3:
        case TextureInternalFormat::BC7:
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

GLenum OpenGLHelpers::ToTextureTarget(TextureType type, CubemapFace cubemapFace)
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
            return GL_TEXTURE_CUBE_MAP_POSITIVE_X + static_cast<int>(cubemapFace);
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

GLenum OpenGLHelpers::ToBlendFactor(BlendFactor factor)
{
    switch (factor)
    {
        case BlendFactor::ZERO:
            return GL_ZERO;
        case BlendFactor::ONE:
            return GL_ONE;
        case BlendFactor::SRC_COLOR:
            return GL_SRC_COLOR;
        case BlendFactor::ONE_MINUS_SRC_COLOR:
            return GL_ONE_MINUS_SRC_COLOR;
        case BlendFactor::DST_COLOR:
            return GL_DST_COLOR;
        case BlendFactor::ONE_MINUS_DST_COLOR:
            return GL_ONE_MINUS_DST_COLOR;
        case BlendFactor::SRC_ALPHA:
            return GL_SRC_ALPHA;
        case BlendFactor::ONE_MINUS_SRC_ALPHA:
            return GL_ONE_MINUS_SRC_ALPHA;
        case BlendFactor::DST_ALPHA:
            return GL_DST_ALPHA;
        case BlendFactor::ONE_MINUS_DST_ALPHA:
            return GL_ONE_MINUS_DST_ALPHA;
        case BlendFactor::CONSTANT_COLOR:
            return GL_CONSTANT_COLOR;
        case BlendFactor::ONE_MINUS_CONSTANT_COLOR:
            return GL_ONE_MINUS_CONSTANT_COLOR;
        case BlendFactor::CONSTANT_ALPHA:
            return GL_CONSTANT_ALPHA;
        case BlendFactor::ONE_MINUS_CONSTANT_ALPHA:
            return GL_ONE_MINUS_CONSTANT_ALPHA;
        case BlendFactor::SRC_ALPHA_SATURATE:
            return GL_SRC_ALPHA_SATURATE;
        case BlendFactor::SRC1_COLOR:
            return GL_SRC1_COLOR;
        case BlendFactor::ONE_MINUS_SRC1_COLOR:
            return GL_ONE_MINUS_SRC1_COLOR;
        case BlendFactor::SRC1_ALPHA:
            return GL_SRC1_ALPHA;
        case BlendFactor::ONE_MINUS_SRC1_ALPHA:
            return GL_ONE_MINUS_SRC1_ALPHA;
    }
}

bool OpenGLHelpers::IsTexture(GLenum uniformType)
{
    return uniformType == GL_SAMPLER_1D ||
           uniformType == GL_SAMPLER_2D ||
           uniformType == GL_SAMPLER_3D ||
           uniformType == GL_SAMPLER_CUBE ||
           uniformType == GL_SAMPLER_1D_SHADOW ||
           uniformType == GL_SAMPLER_2D_SHADOW ||
           uniformType == GL_SAMPLER_1D_ARRAY ||
           uniformType == GL_SAMPLER_2D_ARRAY ||
           uniformType == GL_SAMPLER_1D_ARRAY_SHADOW ||
           uniformType == GL_SAMPLER_2D_ARRAY_SHADOW ||
           uniformType == GL_SAMPLER_2D_MULTISAMPLE ||
           uniformType == GL_SAMPLER_2D_MULTISAMPLE_ARRAY ||
           uniformType == GL_SAMPLER_CUBE_SHADOW ||
           uniformType == GL_SAMPLER_BUFFER ||
           uniformType == GL_SAMPLER_2D_RECT ||
           uniformType == GL_SAMPLER_2D_RECT_SHADOW ||
           uniformType == GL_INT_SAMPLER_1D ||
           uniformType == GL_INT_SAMPLER_2D ||
           uniformType == GL_INT_SAMPLER_3D ||
           uniformType == GL_INT_SAMPLER_CUBE ||
           uniformType == GL_INT_SAMPLER_1D_ARRAY ||
           uniformType == GL_INT_SAMPLER_2D_ARRAY ||
           uniformType == GL_INT_SAMPLER_2D_MULTISAMPLE ||
           uniformType == GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY ||
           uniformType == GL_INT_SAMPLER_BUFFER ||
           uniformType == GL_INT_SAMPLER_2D_RECT ||
           uniformType == GL_UNSIGNED_INT_SAMPLER_1D ||
           uniformType == GL_UNSIGNED_INT_SAMPLER_2D ||
           uniformType == GL_UNSIGNED_INT_SAMPLER_3D ||
           uniformType == GL_UNSIGNED_INT_SAMPLER_CUBE ||
           uniformType == GL_UNSIGNED_INT_SAMPLER_1D_ARRAY ||
           uniformType == GL_UNSIGNED_INT_SAMPLER_2D_ARRAY ||
           uniformType == GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE ||
           uniformType == GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY ||
           uniformType == GL_UNSIGNED_INT_SAMPLER_BUFFER ||
           uniformType == GL_UNSIGNED_INT_SAMPLER_2D_RECT;
}

TextureDataType OpenGLHelpers::FromTextureDataType(GLenum textureDataType)
{
    switch (textureDataType)
    {
        case GL_SAMPLER_1D:
            return TextureDataType::SAMPLER_1D;
        case GL_SAMPLER_2D:
            return TextureDataType::SAMPLER_2D;
        case GL_SAMPLER_3D:
            return TextureDataType::SAMPLER_3D;
        case GL_SAMPLER_CUBE:
            return TextureDataType::SAMPLER_CUBE;
        case GL_SAMPLER_1D_SHADOW:
            return TextureDataType::SAMPLER_1D_SHADOW;
        case GL_SAMPLER_2D_SHADOW:
            return TextureDataType::SAMPLER_2D_SHADOW;
        case GL_SAMPLER_1D_ARRAY:
            return TextureDataType::SAMPLER_1D_ARRAY;
        case GL_SAMPLER_2D_ARRAY:
            return TextureDataType::SAMPLER_2D_ARRAY;
        case GL_SAMPLER_1D_ARRAY_SHADOW:
            return TextureDataType::SAMPLER_1D_ARRAY_SHADOW;
        case GL_SAMPLER_2D_ARRAY_SHADOW:
            return TextureDataType::SAMPLER_2D_ARRAY_SHADOW;
        case GL_SAMPLER_2D_MULTISAMPLE:
            return TextureDataType::SAMPLER_2D_MULTISAMPLE;
        case GL_SAMPLER_2D_MULTISAMPLE_ARRAY:
            return TextureDataType::SAMPLER_2D_MULTISAMPLE_ARRAY;
        case GL_SAMPLER_CUBE_SHADOW:
            return TextureDataType::SAMPLER_CUBE_SHADOW;
        case GL_SAMPLER_BUFFER:
            return TextureDataType::SAMPLER_BUFFER;
        case GL_SAMPLER_2D_RECT:
            return TextureDataType::SAMPLER_2D_RECT;
        case GL_SAMPLER_2D_RECT_SHADOW:
            return TextureDataType::SAMPLER_2D_RECT_SHADOW;
        case GL_INT_SAMPLER_1D:
            return TextureDataType::INT_SAMPLER_1D;
        case GL_INT_SAMPLER_2D:
            return TextureDataType::INT_SAMPLER_2D;
        case GL_INT_SAMPLER_3D:
            return TextureDataType::INT_SAMPLER_3D;
        case GL_INT_SAMPLER_CUBE:
            return TextureDataType::INT_SAMPLER_CUBE;
        case GL_INT_SAMPLER_1D_ARRAY:
            return TextureDataType::INT_SAMPLER_1D_ARRAY;
        case GL_INT_SAMPLER_2D_ARRAY:
            return TextureDataType::INT_SAMPLER_2D_ARRAY;
        case GL_INT_SAMPLER_2D_MULTISAMPLE:
            return TextureDataType::INT_SAMPLER_2D_MULTISAMPLE;
        case GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:
            return TextureDataType::INT_SAMPLER_2D_MULTISAMPLE_ARRAY;
        case GL_INT_SAMPLER_BUFFER:
            return TextureDataType::INT_SAMPLER_BUFFER;
        case GL_INT_SAMPLER_2D_RECT:
            return TextureDataType::INT_SAMPLER_2D_RECT;
        case GL_UNSIGNED_INT_SAMPLER_1D:
            return TextureDataType::UNSIGNED_INT_SAMPLER_1D;
        case GL_UNSIGNED_INT_SAMPLER_2D:
            return TextureDataType::UNSIGNED_INT_SAMPLER_2D;
        case GL_UNSIGNED_INT_SAMPLER_3D:
            return TextureDataType::UNSIGNED_INT_SAMPLER_3D;
        case GL_UNSIGNED_INT_SAMPLER_CUBE:
            return TextureDataType::UNSIGNED_INT_SAMPLER_CUBE;
        case GL_UNSIGNED_INT_SAMPLER_1D_ARRAY:
            return TextureDataType::UNSIGNED_INT_SAMPLER_1D_ARRAY;
        case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY:
            return TextureDataType::UNSIGNED_INT_SAMPLER_2D_ARRAY;
        case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE:
            return TextureDataType::UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE;
        case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:
            return TextureDataType::UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY;
        case GL_UNSIGNED_INT_SAMPLER_BUFFER:
            return TextureDataType::UNSIGNED_INT_SAMPLER_BUFFER;
        case GL_UNSIGNED_INT_SAMPLER_2D_RECT:
            return TextureDataType::UNSIGNED_INT_SAMPLER_2D_RECT;
    }
}

#endif //RENDER_BACKEND_OPENGL