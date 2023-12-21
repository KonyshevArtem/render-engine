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

    bool GetImagePixelsPNG(const std::filesystem::path& path, std::vector<unsigned char>& pixels, TextureHeader& header, int& colorType)
    {
        bool success = lodepng::decode(pixels, header.Width, header.Height, path.string(), static_cast<LodePNGColorType>(colorType)) == 0;
        if (!success)
        {
            Debug::LogError("Error while decoding PNG");
        }

        return success;
    }

    bool GetImagePixelsTGA(const std::filesystem::path& path, std::vector<unsigned char>& pixels, TextureHeader& header, int& colorType)
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

        tga::Image image;
        image.bytesPerPixel = tgaHeader.bytesPerPixel();
        image.rowstride = tgaHeader.width * tgaHeader.bytesPerPixel();

        auto tgaPixels = std::vector<unsigned char>(image.rowstride * tgaHeader.height);
        image.pixels = &tgaPixels[0];

        if (!decoder.readImage(tgaHeader, image, nullptr))
        {
            Debug::LogError("Error while reading TGA pixels");
            return false;
        }

        decoder.postProcessImage(tgaHeader, image);

        header.Width = tgaHeader.width;
        header.Height = tgaHeader.height;

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
                int bitsPerPixel = std::max(tgaHeader.bitsPerPixel, (uint8_t)16);
                int bytesPerPixel = bitsPerPixel / 8;

                pixels = std::vector<unsigned char>(tgaHeader.width * tgaHeader.height * bytesPerPixel);
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

    bool TryGetImagePixels(const std::filesystem::path& path, std::vector<unsigned char>& pixels, TextureHeader& header, int& colorType)
    {
        auto extension = path.extension();

        if (extension == ".png")
        {
            return GetImagePixelsPNG(path, pixels, header, colorType);
        }

        if (extension == ".tga")
        {
            return GetImagePixelsTGA(path, pixels, header, colorType);
        }

        Debug::LogErrorFormat("%1% file extension is not supported", {extension});
        return false;
    }

    int GetMipsCount(bool generateMips, unsigned int height, unsigned int width)
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

        return std::max(std::log2(width), std::log2(height)) + 1;
    }

    void CompressTexture(const std::string& pathString, int colorType, int compressedFormat, bool generateMips)
    {
        constexpr unsigned int headerSize = sizeof(TextureHeader);

        TextureHeader header;
        std::vector<unsigned char> pixels;

        auto path = std::filesystem::path(pathString);
        if (!TryGetImagePixels(path, pixels, header, colorType))
        {
            return;
        }

        header.Format = TextureCompressorFormats::GetFormatByColorType(colorType);
        header.MipCount = GetMipsCount(generateMips, header.Height, header.Width);

        GLuint texture = 0;
        CHECK_GL(glGenTextures(1, &texture));
        CHECK_GL(glBindTexture(GL_TEXTURE_2D, texture));
        CHECK_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0));
        CHECK_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, header.MipCount - 1));
        CHECK_GL(glTexImage2D(GL_TEXTURE_2D, 0, compressedFormat, header.Width, header.Height, 0, header.Format, GL_UNSIGNED_BYTE, pixels.data()));
        if (generateMips)
        {
            CHECK_GL(glGenerateMipmap(GL_TEXTURE_2D));
        }

        CHECK_GL(glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_COMPRESSED, &header.IsCompressed));
        CHECK_GL(glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &header.InternalFormat));

        std::ofstream fout;
        auto outputPath = path.replace_extension("");
        fout.open(outputPath, std::ios::binary | std::ios::out);
        fout.write(reinterpret_cast<char*>(&header), headerSize);

        std::vector<unsigned int> sizes(header.MipCount);
        for (int i = 0; i < header.MipCount; ++i)
        {
            int compressedSize;
            if (header.IsCompressed)
            {
                CHECK_GL(glGetTexLevelParameteriv(GL_TEXTURE_2D, i, GL_TEXTURE_COMPRESSED_IMAGE_SIZE, &compressedSize));
            }
            else
            {
                compressedSize = pixels.size() / (1 << i);
            }
            sizes[i] = compressedSize;
        }
        fout.write(reinterpret_cast<char*>(&sizes[0]), sizes.size() * sizeof(unsigned int));

        unsigned int totalSize = 0;
        std::vector<unsigned char> compressedPixels(sizes[0]);
        for (int i = 0; i < header.MipCount; ++i)
        {
            if (header.IsCompressed)
            {
                CHECK_GL(glGetCompressedTexImage(GL_TEXTURE_2D, i, &compressedPixels[0]));
            }
            else
            {
                CHECK_GL(glGetTexImage(GL_TEXTURE_2D, i, header.Format, GL_UNSIGNED_BYTE, &compressedPixels[0]));
            }

            fout.write(reinterpret_cast<char*>(&compressedPixels[0]), sizes[i]);
            totalSize += sizes[i];
        }

        fout.close();

        std::cout << "\tTexture successfully compressed: " << outputPath
                  << "\n\tFormat: " << TextureCompressorFormats::GetCompressedFormatName(header.InternalFormat) << " (" << header.InternalFormat << ")"
                  << "\n\tOriginal Size: " << GetReadableSize(pixels.size())
                  << "\n\tCompressed Size: " << GetReadableSize(headerSize + totalSize)
                  << "\n\tMipmaps: " << header.MipCount
                  << "\n" << std::endl;

        CHECK_GL(glBindTexture(GL_TEXTURE_2D, 0));
        CHECK_GL(glDeleteTextures(1, &texture));
    }
}