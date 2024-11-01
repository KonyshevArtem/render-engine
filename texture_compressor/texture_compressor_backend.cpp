#include "texture_compressor_backend.h"
#include "texture_compressor_formats.h"

#include <iostream>
#include <vector>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <cmath>

#include "cuttlefish/Image.h"
#include "cuttlefish/Texture.h"

#include "../core/texture/texture_header.h"
#include "debug.h"

namespace TextureCompressorBackend
{
    std::string GetReadableSize(uint32_t bytes)
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

    int GetMipsCount(bool generateMips, uint32_t width, uint32_t height)
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

    bool TryGetTextureTypeInfo(const std::string& textureType, int inputTextureCount, TextureTypeInfo &typeInfo)
    {
        typeInfo = TextureCompressorFormats::GetTextureTypeInfo(textureType);
        if (typeInfo.Count < 0)
        {
            Debug::LogErrorFormat("Texture type %1% is not supported", {textureType});
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

    bool TryGetTextureFormatInfo(const std::string& textureFormat, TextureFormatInfo& outFormatInfo)
    {
        outFormatInfo = TextureCompressorFormats::GetTextureFormatInfo(textureFormat);
        if (outFormatInfo.Format == TextureInternalFormat::INVALID)
        {
            Debug::LogErrorFormat("Texture format %1% is not supported", {textureFormat});
            return false;
        }

        return true;
    }

    bool TryLoadImages(const std::vector<std::string>& pathStrings, bool isLinear, uint32_t slices, std::vector<cuttlefish::Image*> &images)
    {
        images.resize(slices);

        cuttlefish::ColorSpace colorSpace = isLinear ? cuttlefish::ColorSpace::Linear : cuttlefish::ColorSpace::sRGB;
        for (int i = 0; i < slices; i++)
        {
            images[i] = new cuttlefish::Image();
            if (!images[i]->load(pathStrings[i].c_str(), colorSpace))
                return false;
        }

        return true;
    }

    std::vector<uint32_t> ExtractSizes(const cuttlefish::Texture* texture, const TextureHeader& header, bool isCubemap, uint32_t& outTotalSize)
    {
        outTotalSize = 0;
        std::vector<uint32_t> sizes(header.Depth * header.MipCount);
        for (int i = 0; i < header.Depth; ++i)
        {
            for (int j = 0; j < header.MipCount; ++j)
            {
                uint32_t size;
                if (isCubemap)
                {
                    auto face = static_cast<cuttlefish::Texture::CubeFace>(i);
                    size = texture->dataSize(face, j, 0);
                }
                else
                    size = texture->dataSize(j, 0);

                sizes[i * header.MipCount + j] = size;
                outTotalSize += size;
            }
        }
        return sizes;
    }

    void ExtractPixelsAndWriteToFile(const cuttlefish::Texture* texture, const std::vector<uint32_t>& sizes, const TextureHeader& header, bool isCubemap, std::ofstream& fout)
    {
        for (int i = 0; i < header.Depth; ++i)
        {
            for (int j = 0; j < header.MipCount; ++j)
            {
                const void* data;
                if (isCubemap)
                {
                    auto face = static_cast<cuttlefish::Texture::CubeFace>(i);
                    data = texture->data(face, j, 0);
                }
                else
                {
                    data = texture->data(j, 0);
                }

                int size = sizes[i * header.MipCount + j];
                fout.write(static_cast<const char *>(data), size);
            }
        }
    }

    cuttlefish::Texture *CreateTexture(const TextureTypeInfo& typeInfo, const TextureFormatInfo& formatInfo, const TextureHeader& header,
                                       const std::vector<cuttlefish::Image *>& images, bool generateMips, bool isCubemap)
    {
        cuttlefish::Texture* texture = new cuttlefish::Texture(typeInfo.CuttlefishDimensions, header.Width, header.Height, 0,
    header.MipCount, images[0]->colorSpace());

        for (int i = 0; i < header.Depth; ++i)
        {
            if (isCubemap)
            {
                auto face = static_cast<cuttlefish::Texture::CubeFace>(i);
                texture->setImage(*images[i], face);
            }
            else
            {
                texture->setImage(*images[i]);
            }
        }

        if (generateMips)
        {
            texture->generateMipmaps();
        }

        texture->convert(formatInfo.CuttlefishFormat, formatInfo.CuttlefishType);

        return texture;
    }

    void CompressTexture(const std::vector<std::string>& paths, const std::string& textureType, const std::string& textureFormat,
                        bool isLinear, bool generateMips, const std::string& outputName)
    {
        constexpr uint32_t headerSize = sizeof(TextureHeader);

        TextureTypeInfo typeInfo;
        TextureFormatInfo formatInfo;
        if (!TryGetTextureFormatInfo(textureFormat, formatInfo) ||
            !TryGetTextureTypeInfo(textureType, paths.size(), typeInfo))
        {
            return;
        }

        std::vector<cuttlefish::Image*> images;
        if (!TryLoadImages(paths, isLinear, typeInfo.Count, images))
        {
            return;
        }

        TextureHeader header{};
        header.Depth = typeInfo.Count;
        header.Width = images[0]->width();
        header.Height = images[0]->height();
        header.MipCount = GetMipsCount(generateMips, header.Width, header.Height);
        header.TextureFormat = formatInfo.Format;
        header.IsLinear = isLinear;

        bool isCubemap = typeInfo.CuttlefishDimensions == cuttlefish::Texture::Dimension::Cube;
        cuttlefish::Texture* texture = CreateTexture(typeInfo, formatInfo, header, images, generateMips, isCubemap);

        uint32_t totalCompressedSize;
        std::vector<uint32_t> compressedSizes = ExtractSizes(texture, header, isCubemap, totalCompressedSize);

        std::filesystem::path outputPath = std::filesystem::path(paths[0]).parent_path() / "output" / outputName;
        std::filesystem::create_directory(outputPath.parent_path());

        std::ofstream fout;
        fout.open(outputPath, std::ios::binary | std::ios::out);
        fout.write(reinterpret_cast<char*>(&header), headerSize);
        fout.write(reinterpret_cast<char*>(&compressedSizes[0]), compressedSizes.size() * sizeof(uint32_t));

        ExtractPixelsAndWriteToFile(texture, compressedSizes, header, isCubemap, fout);
        fout.close();

        std::cout << "\tTexture successfully compressed: " << outputPath
                  << "\n\tFormat: " << formatInfo.Name << " (" << static_cast<int>(header.TextureFormat) << ")"
                  << "\n\tCompressed Size: " << GetReadableSize(headerSize + totalCompressedSize)
                  << "\n\tMipmaps: " << static_cast<int>(header.MipCount)
                  << "\n" << std::endl;

        delete texture;
        for (int i = 0; i < images.size(); ++i)
        {
            delete images[i];
        }
    }
}