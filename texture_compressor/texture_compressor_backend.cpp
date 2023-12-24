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
#include <sstream>
#include <cmath>

#include "../core/texture/texture_header.h"
#include "texture_compressor_formats.h"
#include "lodepng.h"
#include "tga.h"
#include "debug.h"

namespace TextureCompressorBackend
{
    std::string GetReadableSize(unsigned int bytes)
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

    bool GetImagePixelsPNG(const std::filesystem::path &path, std::vector<uint8_t> &pixels, unsigned int &width, unsigned int &height, int &colorType)
    {
        bool success = lodepng::decode(pixels, width, height, path.string(), static_cast<LodePNGColorType>(colorType)) == 0;
        if (!success)
        {
            Debug::LogError("Error while decoding PNG");
        }

        return success;
    }

    bool GetImagePixelsTGA(const std::filesystem::path &path, std::vector<uint8_t> &pixels, unsigned int &width, unsigned int &height, int &colorType)
    {
        FILE* f = std::fopen(path.c_str(), "rb");
        tga::StdioFileInterface file(f);
        tga::Decoder decoder(&file);
        tga::Header tgaHeader;
        if (!decoder.readHeader(tgaHeader))
        {
            Debug::LogError("Error while reading TGA header");
            return false;
        }

        tga::Image image{};
        image.bytesPerPixel = tgaHeader.bytesPerPixel();
        image.rowstride = tgaHeader.width * tgaHeader.bytesPerPixel();

        auto tgaPixels = std::vector<uint8_t>(image.rowstride * tgaHeader.height);
        image.pixels = &tgaPixels[0];

        if (!decoder.readImage(tgaHeader, image, nullptr))
        {
            Debug::LogError("Error while reading TGA pixels");
            return false;
        }

        decoder.postProcessImage(tgaHeader, image);

        width = tgaHeader.width;
        height = tgaHeader.height;

        if (tgaHeader.isGray())
        {
            colorType = decoder.hasAlpha() ? LCT_GREY_ALPHA : LCT_GREY;
            pixels = std::move(tgaPixels);
        }
        else if (tgaHeader.isRgb())
        {
            colorType = decoder.hasAlpha() ? LCT_RGBA : LCT_RGB;

            if (tgaHeader.bitsPerPixel == 32)
            {
                pixels = std::move(tgaPixels);
            }
            else
            {
                int bitsPerPixel = std::max(tgaHeader.bitsPerPixel, static_cast<uint8_t>(16));
                int bytesPerPixel = bitsPerPixel / 8;

                pixels = std::vector<uint8_t>(tgaHeader.width * tgaHeader.height * bytesPerPixel);
                size_t src = 0;
                size_t dst = 0;
                size_t size = tgaPixels.size();
                while (src < size)
                {
                    memcpy(&pixels[0] + dst, &tgaPixels[0] + src, bytesPerPixel);
                    src += 4;
                    dst += bytesPerPixel;
                }
            }
        }

        return true;
    }

    bool TryGetImagePixels(const std::filesystem::path &path, std::vector<uint8_t> &pixels, unsigned int &width, unsigned int &height, int &colorType)
    {
        auto extension = path.extension();

        if (extension == ".png")
        {
            return GetImagePixelsPNG(path, pixels, width, height, colorType);
        }

        if (extension == ".tga")
        {
            return GetImagePixelsTGA(path, pixels, width, height, colorType);
        }

        Debug::LogErrorFormat("%1% file extension is not supported", {extension});
        return false;
    }

    int GetMipsCount(bool generateMips, unsigned int width, unsigned int height)
    {
        if (!generateMips)
        {
            return 1;
        }

        if (width % 2 != 0 || height % 2 != 0)
        {
            Debug::LogError("Texture size is not power of 2");
            return 1;
        }

        return std::max(static_cast<int>(std::log2(width)),
                        static_cast<int>(std::log2(height))) + 1;
    }

    bool TryGetTextureTypeInfo(int textureType, int inputTextureCount, TextureTypeInfo &typeInfo)
    {
        typeInfo = TextureCompressorFormats::GetTextureTypeInfo(textureType);
        if (typeInfo.Count < 0)
        {
            Debug::LogErrorFormat("Texture type %1% is not supported", {std::to_string(textureType)});
            return false;
        }

        if (inputTextureCount < typeInfo.Count)
        {
            Debug::LogErrorFormat("Provided textures count (%1%) is not enough for provided texture type (%2%). Required textures count is %3%",
                                  {std::to_string(inputTextureCount), typeInfo.Name, std::to_string(typeInfo.Count)});
            return false;
        }

        return true;
    }

    int GetTextureTarget(int textureType, int slice)
    {
        if (textureType == GL_TEXTURE_CUBE_MAP)
        {
            return GL_TEXTURE_CUBE_MAP_POSITIVE_X + slice;
        }

        return GL_TEXTURE_2D;
    }

    bool TryLoadImagesAndSendToGPU(const std::vector<std::string> &pathStrings, int textureType, unsigned int &width, unsigned int &height,
                                   unsigned int depth, int &colorType, int compressedFormat, int &format, std::vector<int> &originalSizes)
    {
        originalSizes.resize(depth);

        std::vector<uint8_t> pixels;
        for (int i = 0; i < depth; i++)
        {
            auto path = std::filesystem::path(pathStrings[i]);
            if (TryGetImagePixels(path, pixels, width, height, colorType))
            {
                format = TextureCompressorFormats::GetFormatByColorType(colorType);

                int uploadTarget = GetTextureTarget(textureType, i);
                CHECK_GL(glTexImage2D(uploadTarget, 0, compressedFormat, width, height, 0, format, GL_UNSIGNED_BYTE, pixels.data()))

                originalSizes[i] = pixels.size();
                pixels.clear();
            }
            else
            {
                return false;
            }
        }

        return true;
    }

    std::vector<unsigned int> ExtractCompressedSizes(int textureType, const TextureHeader &header, const std::vector<int> &originalSizes)
    {
        std::vector<unsigned int> sizes(header.Depth * header.MipCount);
        for (int i = 0; i < header.Depth; ++i)
        {
            int textureTarget = GetTextureTarget(textureType, i);
            for (int j = 0; j < header.MipCount; ++j)
            {
                int compressedSize;
                if (header.IsCompressed)
                {
                    CHECK_GL(glGetTexLevelParameteriv(textureTarget, j, GL_TEXTURE_COMPRESSED_IMAGE_SIZE, &compressedSize))
                }
                else
                {
                    compressedSize = originalSizes[i] / (1 << j);
                }
                sizes[i * header.MipCount + j] = compressedSize;
            }
        }
        return sizes;
    }

    void ExtractPixelsAndWriteToFile(int textureType, const std::vector<unsigned int> &sizes, const TextureHeader &header, unsigned int &totalSize, std::ofstream &fout)
    {
        std::vector<uint8_t> compressedPixels(sizes[0]);
        for (int i = 0; i < header.Depth; ++i)
        {
            int textureTarget = GetTextureTarget(textureType, i);
            for (int j = 0; j < header.MipCount; ++j)
            {
                if (header.IsCompressed)
                {
                    CHECK_GL(glGetCompressedTexImage(textureTarget, j, &compressedPixels[0]))
                }
                else
                {
                    CHECK_GL(glGetTexImage(textureTarget, j, header.Format, GL_UNSIGNED_BYTE, &compressedPixels[0]))
                }

                int size = sizes[i * header.MipCount + j];
                fout.write(reinterpret_cast<char *>(&compressedPixels[0]), size);
                totalSize += size;
            }
        }
    }

    void CompressTexture(const std::vector<std::string> &pathStrings, int textureType, int colorType, int compressedFormat, bool generateMips)
    {
        constexpr unsigned int headerSize = sizeof(TextureHeader);

        TextureTypeInfo typeInfo;
        if (!TryGetTextureTypeInfo(textureType, pathStrings.size(), typeInfo))
        {
            return;
        }

        TextureHeader header{};
        header.Depth = typeInfo.Count;

        GLuint texture = 0;
        CHECK_GL(glGenTextures(1, &texture))
        CHECK_GL(glBindTexture(textureType, texture))

        std::vector<int> originalSizes;
        if (!TryLoadImagesAndSendToGPU(pathStrings, textureType, header.Width, header.Height, header.Depth,
                                       colorType, compressedFormat, header.Format, originalSizes))
        {
            return;
        }

        header.MipCount = GetMipsCount(generateMips, header.Width, header.Height);
        CHECK_GL(glTexParameteri(textureType, GL_TEXTURE_BASE_LEVEL, 0))
        CHECK_GL(glTexParameteri(textureType, GL_TEXTURE_MAX_LEVEL, header.MipCount - 1))
        if (generateMips)
        {
            CHECK_GL(glGenerateMipmap(textureType))
        }

        int baseTarget = GetTextureTarget(textureType, 0);
        CHECK_GL(glGetTexLevelParameteriv(baseTarget, 0, GL_TEXTURE_COMPRESSED, &header.IsCompressed))
        CHECK_GL(glGetTexLevelParameteriv(baseTarget, 0, GL_TEXTURE_INTERNAL_FORMAT, &header.InternalFormat))

        std::ofstream fout;
        auto outputPath = std::filesystem::path(pathStrings[0]).replace_extension("");
        fout.open(outputPath, std::ios::binary | std::ios::out);
        fout.write(reinterpret_cast<char*>(&header), headerSize);

        std::vector<unsigned int> sizes = ExtractCompressedSizes(textureType, header, originalSizes);
        fout.write(reinterpret_cast<char*>(&sizes[0]), sizes.size() * sizeof(unsigned int));

        unsigned int totalCompressedSize;
        ExtractPixelsAndWriteToFile(textureType, sizes, header, totalCompressedSize, fout);
        fout.close();

        unsigned int totalOriginalSize = 0;
        for (int size: originalSizes)
        {
            totalOriginalSize += size;
        }

        std::cout << "\tTexture successfully compressed: " << outputPath
                  << "\n\tFormat: " << TextureCompressorFormats::GetCompressedFormatName(header.InternalFormat) << " (" << header.InternalFormat << ")"
                  << "\n\tOriginal Size: " << GetReadableSize(totalOriginalSize)
                  << "\n\tCompressed Size: " << GetReadableSize(headerSize + totalCompressedSize)
                  << "\n\tMipmaps: " << header.MipCount
                  << "\n" << std::endl;

        CHECK_GL(glBindTexture(textureType, 0))
        CHECK_GL(glDeleteTextures(1, &texture))
    }
}