#include "texture_compressor_backend.h"
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
#include "graphics_backend_api.h"
#include "types/graphics_backend_texture.h"
#include "helpers/opengl_helpers.h"

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

    bool GetImagePixelsPNG(const std::filesystem::path &path, std::vector<uint8_t> &pixels, int &width, int &height, int &colorType)
    {
        unsigned int unsignedWidth;
        unsigned int unsignedHeight;
        bool success = lodepng::decode(pixels, unsignedWidth, unsignedHeight, path.string(), static_cast<LodePNGColorType>(colorType)) == 0;
        if (!success)
        {
            Debug::LogError("Error while decoding PNG");
        }

        width = unsignedWidth;
        height = unsignedHeight;
        return success;
    }

    bool GetImagePixelsTGA(const std::filesystem::path &path, std::vector<uint8_t> &pixels, int &width, int &height, int &colorType)
    {
        FILE* f = std::fopen(path.string().c_str(), "rb");
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

    bool TryGetImagePixels(const std::filesystem::path &path, std::vector<uint8_t> &pixels, int &width, int &height, int &colorType)
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

        Debug::LogErrorFormat("%1% file extension is not supported", {extension.string()});
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

    bool TryGetTextureTypeInfo(TextureType textureType, int inputTextureCount, TextureTypeInfo &typeInfo)
    {
        typeInfo = TextureCompressorFormats::GetTextureTypeInfo(textureType);
        if (typeInfo.Count < 0)
        {
            Debug::LogErrorFormat("Texture type %1% is not supported", {std::to_string(static_cast<int>(textureType))});
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

    bool TryLoadImages(const std::vector<std::string> &pathStrings, int &width, int &height, unsigned int slices,
                       int &colorType, std::vector<std::vector<uint8_t>> &images)
    {
        images.resize(slices);

        for (int i = 0; i < slices; i++)
        {
            auto path = std::filesystem::path(pathStrings[i]);
            if (!TryGetImagePixels(path, images[i], width, height, colorType))
            {
                return false;
            }
        }

        return true;
    }

    void UploadImagesToGPU(const GraphicsBackendTexture &texture, int width, int height, const std::vector<std::vector<uint8_t>> &images)
    {
        for (int i = 0; i < images.size(); ++i)
        {
            // pass images size 0 so OpenGL uploads using glTexImage for automatic compression
            GraphicsBackend::Current()->UploadImagePixels(texture, 0, i, width, height, 0, 0, images[i].data());
        }
    }

    std::vector<unsigned int> ExtractCompressedSizes(const GraphicsBackendTexture &texture, const TextureHeader &header,
                                                     const std::vector<std::vector<uint8_t>> &images)
    {
        std::vector<unsigned int> sizes(header.Depth * header.MipCount);
        for (int i = 0; i < header.Depth; ++i)
        {
            for (int j = 0; j < header.MipCount; ++j)
            {
                int compressedSize = GraphicsBackend::Current()->GetTextureSize(texture, j, i);
                if (compressedSize == 0)
                {
                    compressedSize = images[i].size() / std::pow(4, j);
                }
                sizes[i * header.MipCount + j] = compressedSize;
            }
        }
        return sizes;
    }

    void ExtractPixelsAndWriteToFile(const GraphicsBackendTexture &texture, const std::vector<unsigned int> &sizes, const TextureHeader &header, unsigned int &totalSize, std::ofstream &fout)
    {
        std::vector<uint8_t> compressedPixels(sizes[0]);
        for (int i = 0; i < header.Depth; ++i)
        {
            for (int j = 0; j < header.MipCount; ++j)
            {
                GraphicsBackend::Current()->DownloadImagePixels(texture, j, i, &compressedPixels[0]);

                int size = sizes[i * header.MipCount + j];
                fout.write(reinterpret_cast<char *>(&compressedPixels[0]), size);
                totalSize += size;
            }
        }
    }

    void CompressTexture(const std::vector<std::string> &pathStrings, TextureType textureType, int colorType, TextureInternalFormat textureFormat, bool generateMips)
    {
        constexpr unsigned int headerSize = sizeof(TextureHeader);

        TextureTypeInfo typeInfo;
        if (!TryGetTextureTypeInfo(textureType, pathStrings.size(), typeInfo))
        {
            return;
        }

        TextureHeader header{};
        header.Depth = typeInfo.Count;

        std::vector<std::vector<uint8_t>> images;
        if (!TryLoadImages(pathStrings, header.Width, header.Height, header.Depth, colorType, images))
        {
            return;
        }

        header.MipCount = GetMipsCount(generateMips, header.Width, header.Height);

        GraphicsBackendTexture texture = GraphicsBackend::Current()->CreateTexture(0, 0, textureType, textureFormat, header.MipCount, false);
        UploadImagesToGPU(texture, header.Width, header.Height, images);

        if (generateMips)
        {
            GraphicsBackend::Current()->GenerateMipmaps(texture);
        }

        header.TextureFormat = GraphicsBackend::Current()->GetTextureFormat(texture);

        std::ofstream fout;
        auto outputPath = std::filesystem::path(pathStrings[0]).replace_extension("");
        fout.open(outputPath, std::ios::binary | std::ios::out);
        fout.write(reinterpret_cast<char*>(&header), headerSize);

        std::vector<unsigned int> sizes = ExtractCompressedSizes(texture, header, images);
        fout.write(reinterpret_cast<char*>(&sizes[0]), sizes.size() * sizeof(unsigned int));

        unsigned int totalCompressedSize;
        ExtractPixelsAndWriteToFile(texture, sizes, header, totalCompressedSize, fout);
        fout.close();

        unsigned int totalOriginalSize = 0;
        for (auto &img: images)
        {
            totalOriginalSize += img.size();
        }

        std::cout << "\tTexture successfully compressed: " << outputPath
                  << "\n\tFormat: " << TextureCompressorFormats::GetTextureFormatName(header.TextureFormat) << " (" << static_cast<int>(header.TextureFormat) << ")"
                  << "\n\tOriginal Size: " << GetReadableSize(totalOriginalSize)
                  << "\n\tCompressed Size: " << GetReadableSize(headerSize + totalCompressedSize)
                  << "\n\tMipmaps: " << header.MipCount
                  << "\n" << std::endl;

        GraphicsBackend::Current()->DeleteTexture(texture);
    }
}