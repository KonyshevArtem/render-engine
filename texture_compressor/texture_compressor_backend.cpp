#include "texture_compressor_backend.h"
#ifdef TEXTURE_COMPRESSOR_WINDOWS
#include <GL/glew.h>
#elif TEXTURE_COMPRESSOR_MACOS
#include <OpenGL/glu.h>
#endif
#include <iostream>
#include <vector>
#include <filesystem>
#include <fstream>
#include <unordered_map>
#include <sstream>

#include "../core/texture/texture_header.h"
#include "lodepng.h"
#include "debug.h"

namespace TextureCompressorBackend
{
    std::vector<std::pair<int, std::string>> m_InputFormats =
    {
        {LCT_RGB, "RGB"},
        {LCT_RGBA, "RGBA"},
        {LCT_GREY, "GREY"},
        {LCT_GREY_ALPHA, "GREY ALPHA"},
    };

    std::unordered_map<int, int> m_ColorTypeToFormat = 
    {
        {LCT_RGB, GL_RGB},
        {LCT_RGBA, GL_RGBA},
        {LCT_GREY, GL_RED},
        {LCT_GREY_ALPHA, GL_RG}
    };

    std::vector<std::pair<int, std::string>> m_CompressedFormats = 
    {
        {GL_RGB, "RGB"},
        {GL_RGBA, "RGBA"},
        {GL_SRGB, "sRGB"},
        {GL_SRGB_ALPHA, "sRGBA"},
        {GL_COMPRESSED_RGB, "Compressed RGB"},
        {GL_COMPRESSED_RGBA, "Compressed RGBA"},
        {GL_COMPRESSED_SRGB, "Compressed sRGB"},
        {GL_COMPRESSED_SRGB_ALPHA, "Compressed sRGBA"},

        #if GL_ARB_texture_rg
        {GL_COMPRESSED_RED, "Compressed Red"},
        {GL_COMPRESSED_RG, "Compressed RG"},
        #endif

        {GL_R3_G3_B2, "R3 G3 B2"},
        {GL_RGB4, "RGB4"},
        {GL_RGB5, "RGB5"},
        {GL_RGB8, "RGB8"},
        {GL_RGB10, "RGB10"},
        {GL_RGB12, "RGB12"},
        {GL_RGB16, "RGB16"},
        {GL_RGBA2, "RGBA2"},
        {GL_RGBA4, "RGBA4"},
        {GL_RGB5_A1, "RGB5 A1"},
        {GL_RGBA8, "RGBA8"},
        {GL_RGB10_A2, "RGB10 A2"},
        {GL_RGBA12, "RGBA12"},
        {GL_RGBA16, "RGBA16"},

        #if GL_EXT_texture_compression_s3tc
        {GL_COMPRESSED_RGB_S3TC_DXT1_EXT, "RGB S3TC DXT1"},
        {GL_COMPRESSED_RGBA_S3TC_DXT1_EXT, "RGBA S3TC DXT1"},
        {GL_COMPRESSED_RGBA_S3TC_DXT3_EXT, "RGBA S3TC DXT3"},
        {GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, "RGBA S3TC DXT5"},
        #endif

        #if GL_EXT_texture_sRGB
        {GL_COMPRESSED_SRGB_S3TC_DXT1_EXT, "SRGB S3TC DXT1"},
        {GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT, "sRGBA S3TC DXT1"},
        {GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT, "sRGBA S3TC DXT3"},
        {GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT, "sRGBA S3TC DXT5"},
        #endif

        #if GL_ARB_texture_compression_rgtc
        {GL_COMPRESSED_RED_RGTC1, "RED RGTC1"},
        {GL_COMPRESSED_SIGNED_RED_RGTC1, "SIGNED RED RGTC1"},
        {GL_COMPRESSED_RG_RGTC2, "RG RGTC2"},
        {GL_COMPRESSED_SIGNED_RG_RGTC2, "SIGNED RG RGTC2"},
        #endif
    };

    std::string GetFormatName(int format)
    {
        for (const auto & pair : m_CompressedFormats)
        {
            if (pair.first == format)
            {
                return pair.second;
            }
        }

        return "Unknown";
    }

    std::string GetReadableSize(int bytes)
    {
        std::stringstream stream;

        std::string units = "mb";
        float converted = static_cast<float>(bytes) / 1024 / 1024; // mb
        if (converted < 1)
        {
            units = "kb";
            converted = static_cast<float>(bytes) / 1024; // kb
        }

        stream << std::fixed << std::setprecision(2) << converted << " " <<  units;
        return stream.str();
    }

    void CompressTexture(const std::string& path, int colorType, int compressedFormat)
    {
        constexpr int headerSize = sizeof(TextureHeader);

        TextureHeader header;
        std::vector<unsigned char> pixels;

        lodepng::decode(pixels, header.Width, header.Height, path, static_cast<LodePNGColorType>(colorType));

        header.Format = m_ColorTypeToFormat[colorType];

        GLuint texture = 0;
        CHECK_GL(glGenTextures(1, &texture));
        CHECK_GL(glBindTexture(GL_TEXTURE_2D, texture));
        CHECK_GL(glTexImage2D(GL_TEXTURE_2D, 0, compressedFormat, header.Width, header.Height, 0, header.Format, GL_UNSIGNED_BYTE, pixels.data()));

        CHECK_GL(glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_COMPRESSED, &header.IsCompressed));
        CHECK_GL(glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &header.InternalFormat));

        int totalSize;
        unsigned char *compressedPixels;

        int compressedSize;
        if (header.IsCompressed)
        {
            CHECK_GL(glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_COMPRESSED_IMAGE_SIZE, &compressedSize));
        }
        else
        {
            compressedSize = pixels.size() * sizeof(char);
        }

        totalSize = headerSize + compressedSize;
        compressedPixels = new unsigned char[totalSize];

        if (header.IsCompressed)
        {
            CHECK_GL(glGetCompressedTexImage(GL_TEXTURE_2D, 0, compressedPixels + headerSize));
        }
        else
        {
            memcpy(compressedPixels + headerSize, pixels.data(), pixels.size());
        }

        memcpy(compressedPixels, reinterpret_cast<void*>(&header), headerSize);

        std::ofstream fout;
        auto outputPath = std::filesystem::path(path).replace_extension("");
        fout.open(outputPath, std::ios::binary | std::ios::out);
        fout.write(reinterpret_cast<char*>(compressedPixels), totalSize * sizeof(char));
        fout.close();

        delete[] compressedPixels;

        std::cout << "\tTexture successfully compressed"
                  << "\n\tFormat: " << GetFormatName(header.InternalFormat) << " (" << header.InternalFormat << ")"
                  << "\n\tOriginal Size: " << GetReadableSize(pixels.size() * sizeof(char))
                  << "\n\tCompressed Size: " << GetReadableSize(totalSize) << std::endl;

        CHECK_GL(glBindTexture(GL_TEXTURE_2D, 0));
        CHECK_GL(glDeleteTextures(1, &texture));
    }

    const std::vector<std::pair<int, std::string>> &GetInputFormats()
    {
        return m_InputFormats;
    }

    const std::vector<std::pair<int, std::string>> &GetCompressedFormats()
    {
        return m_CompressedFormats;
    }
}