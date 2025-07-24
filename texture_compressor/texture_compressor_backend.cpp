#include "texture_compressor_backend.h"
#include "texture_compressor_formats.h"

#include <vector>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <cmath>
#include <execution>

#include "cuttlefish/Image.h"
#include "cuttlefish/Texture.h"

#include "nlohmann/json.hpp"

#include "graphics_backend_api.h"
#include "../core/texture/texture_header.h"
#include "debug.h"

namespace TextureCompressorBackend
{
    struct FormatsData
    {
        std::string Windows;
        std::string Android;
        std::string Mac;
        std::string iOS;
    };

    struct TextureData
    {
        std::vector<std::string> Paths;
        std::string Type;
        FormatsData Formats;
        bool Linear;
        bool Mips;
        bool FlipY;
    };

    void from_json(const nlohmann::json& json, FormatsData& formats)
    {
        json.at("Windows").get_to(formats.Windows);
        json.at("Android").get_to(formats.Android);
        json.at("Mac").get_to(formats.Mac);
        json.at("iOS").get_to(formats.iOS);
    }

    void from_json(const nlohmann::json& json, TextureData& data)
    {
        json.at("Paths").get_to(data.Paths);
        json.at("Type").get_to(data.Type);
        json.at("Formats").get_to(data.Formats);
        json.at("Linear").get_to(data.Linear);
        json.at("Mips").get_to(data.Mips);

        if (json.contains("FlipY"))
            json.at("FlipY").get_to(data.FlipY);
        else
            data.FlipY = false;
    }

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

    int GetMipsCount(bool generateMips, uint32_t width, uint32_t height, const std::string& texturePath)
    {
        if (!generateMips || width == 1 && height == 1)
        {
            return 1;
        }

        if (width % 2 != 0 || height % 2 != 0)
        {
            Debug::LogErrorFormat("Texture size is not power of 2: {}", texturePath);
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
            Debug::LogErrorFormat("Texture type {} is not supported", textureType);
            return false;
        }

        if (inputTextureCount < typeInfo.Count)
        {
            Debug::LogErrorFormat("Provided textures count ({}) is not enough for provided texture type ({}). Required textures count is {}",
                                  std::to_string(inputTextureCount), typeInfo.Name, std::to_string(typeInfo.Count));
            return false;
        }

        return true;
    }

    bool TryGetTextureFormatInfo(const std::string& textureFormat, TextureFormatInfo& outFormatInfo)
    {
        outFormatInfo = TextureCompressorFormats::GetTextureFormatInfo(textureFormat);
        if (outFormatInfo.Format == TextureInternalFormat::INVALID)
        {
            Debug::LogErrorFormat("Texture format {} is not supported", textureFormat);
            return false;
        }

        return true;
    }

    bool TryLoadImages(const TextureData& data, TextureInternalFormat format, uint32_t slices, std::vector<cuttlefish::Image*>& images)
    {
        images.resize(slices);

        cuttlefish::ColorSpace colorSpace = data.Linear ? cuttlefish::ColorSpace::Linear : cuttlefish::ColorSpace::sRGB;
        for (int i = 0; i < slices; i++)
        {
            images[i] = new cuttlefish::Image();
            if (!images[i]->load(data.Paths[i].c_str(), colorSpace))
            {
                Debug::LogErrorFormat("Cannot load texture: {}", data.Paths[i].c_str());
                return false;
            }

            if (data.FlipY)
                images[i]->flipVertical();

            uint32_t blockSize = GraphicsBackend::Current()->GetBlockSize(format);
            if (blockSize > 0 && images[i]->width() < blockSize || images[i]->height() < blockSize)
            {
                uint32_t blockSizePow2 = std::bit_ceil(blockSize);
                uint32_t newWidth = std::max(blockSizePow2, images[i]->width());
                uint32_t newHeight = std::max(blockSizePow2, images[i]->height());

                cuttlefish::Image* resizedImage = new cuttlefish::Image(images[i]->resize(newWidth, newHeight, cuttlefish::Image::ResizeFilter::Linear));
                delete images[i];
                images[i] = resizedImage;
            }
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

    std::string GetFormat(const TextureData& data, const std::string& platform)
    {
        if (platform == "windows")
            return data.Formats.Windows;
        if (platform == "android")
            return data.Formats.Android;
        if (platform == "mac")
            return data.Formats.Mac;
        if (platform == "ios")
            return data.Formats.iOS;

        Debug::LogErrorFormat("Unknown platform: {}", platform);
        return "";
    }

    void CompressTexture(const TextureData& data, const std::filesystem::path& outputPath, const std::string& platform)
    {
        constexpr uint32_t headerSize = sizeof(TextureHeader);

        std::string textureFormat = GetFormat(data, platform);

        TextureTypeInfo typeInfo;
        TextureFormatInfo formatInfo;
        if (!TryGetTextureFormatInfo(textureFormat, formatInfo) ||
            !TryGetTextureTypeInfo(data.Type, data.Paths.size(), typeInfo))
        {
            return;
        }

        std::vector<cuttlefish::Image*> images;
        if (!TryLoadImages(data, formatInfo.Format, typeInfo.Count, images))
        {
            return;
        }

        TextureHeader header{};
        header.Depth = typeInfo.Count;
        header.Width = images[0]->width();
        header.Height = images[0]->height();
        header.MipCount = GetMipsCount(data.Mips, header.Width, header.Height, outputPath.string());
        header.TextureFormat = formatInfo.Format;
        header.IsLinear = data.Linear;

        bool isCubemap = typeInfo.CuttlefishDimensions == cuttlefish::Texture::Dimension::Cube;
        cuttlefish::Texture* texture = CreateTexture(typeInfo, formatInfo, header, images, data.Mips, isCubemap);

        uint32_t totalCompressedSize;
        std::vector<uint32_t> compressedSizes = ExtractSizes(texture, header, isCubemap, totalCompressedSize);

        std::filesystem::create_directories(outputPath.parent_path());

        std::ofstream fout;
        fout.open(outputPath, std::ios::binary | std::ios::out);
        fout.write(reinterpret_cast<char*>(&header), headerSize);
        fout.write(reinterpret_cast<char*>(&compressedSizes[0]), compressedSizes.size() * sizeof(uint32_t));

        ExtractPixelsAndWriteToFile(texture, compressedSizes, header, isCubemap, fout);
        fout.close();

        Debug::LogInfoFormat("Compressed: {} {} {}", outputPath.string(), formatInfo.Name, GetReadableSize(headerSize + totalCompressedSize));

        delete texture;
        for (int i = 0; i < images.size(); ++i)
            delete images[i];
    }

    void CompressTextures(const std::filesystem::path& inputPath, const std::filesystem::path& outputPath, const std::string& platform)
    {
        std::vector<std::filesystem::path> textureFilePaths;

        for (const std::filesystem::directory_entry& entry: std::filesystem::recursive_directory_iterator(inputPath))
        {
            if (entry.is_regular_file() && entry.path().extension() == ".texture")
                textureFilePaths.push_back(entry.path());
        }

        std::for_each(std::execution::par, textureFilePaths.begin(), textureFilePaths.end(), [&inputPath, &outputPath, &platform](std::filesystem::path& path)
        {
            std::ifstream file(path);
            if (!file)
            {
                std::string pathStr = path.string();
                Debug::LogErrorFormat("Cannot read texture: {}", pathStr);
                return;
            }

            std::stringstream buffer;
            buffer << file.rdbuf();

            TextureData data;
            nlohmann::json textureJson = nlohmann::json::parse(buffer.str());
            textureJson.get_to(data);

            for (std::string& texturePath : data.Paths)
                texturePath = (path.parent_path() / texturePath).string();

            std::filesystem::path relativePath = std::filesystem::relative(path, inputPath);
            std::filesystem::path outputTexturePath = outputPath / relativePath.parent_path() / relativePath.stem();

            CompressTexture(data, outputTexturePath, platform);
            file.close();
        });
    }
}