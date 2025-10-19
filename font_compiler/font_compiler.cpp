#include "arguments.h"

#include "../core/texture/texture_header.h"
#include "../core/font/font_data.h"
#include "cuttlefish/Image.h"
#include "cuttlefish/Texture.h"
#include "debug.h"

#include <fstream>
#include <filesystem>

enum BlockType : uint8_t
{
    INFO = 1,
    COMMON,
    PAGES,
    CHARS,
    KERNING_PAIRS
};

template<typename T>
T ReadData(const uint8_t* bytes, uint32_t& offset)
{
    const T data = *reinterpret_cast<const T*>(&bytes[offset]);
    offset += sizeof(data);
    return data;
}

CommonBlock ReadCommonBlock(const uint8_t* data)
{
    uint32_t offset = 0;

    CommonBlock block{};
    block.LineHeight = ReadData<uint16_t>(data, offset);
    block.Base = ReadData<uint16_t>(data, offset);
    block.ScaleW = ReadData<uint16_t>(data, offset);
    block.ScaleH = ReadData<uint16_t>(data, offset);
    block.Pages = ReadData<uint16_t>(data, offset);
    block.BitField = ReadData<uint8_t>(data, offset);
    block.AlphaChannel = ReadData<uint8_t>(data, offset);
    block.RedChannel = ReadData<uint8_t>(data, offset);
    block.GreenChannel = ReadData<uint8_t>(data, offset);
    block.BlueChannel = ReadData<uint8_t>(data, offset);
    return block;
}

void ReadPagesBlock(const uint8_t* data, uint16_t pageCount, std::vector<const char*>& pageNames)
{
    pageNames.reserve(pageCount);

    uint32_t offset = 0;
    for (int i = 0; i < pageCount; ++i)
    {
        const char* pageName = reinterpret_cast<const char*>(&data[offset]);
        pageNames.push_back(pageName);

        offset += strlen(pageName);
    }
}

void ReadCharBlock(const uint8_t* data, uint32_t blockSize, std::vector<Char>& chars)
{
    const uint32_t charCount = blockSize / sizeof(Char);
    chars.reserve(charCount);

    uint32_t offset = 0;
    for (int i = 0; i < charCount; ++i)
    {
        const Char ch = ReadData<const Char>(data, offset);
        chars.push_back(ch);
    }
}

void ReadKerningBlock(const uint8_t* data, uint32_t blockSize, std::vector<KerningPair>& kerningPairs)
{
    uint32_t offset = 0;
    while (offset < blockSize)
    {
        KerningPair pair{};
        pair.First = ReadData<uint32_t>(data, offset);
        pair.Second = ReadData<uint32_t>(data, offset);
        pair.Amount = ReadData<int16_t>(data, offset);

        kerningPairs.push_back(pair);
    }
}

TextureInternalFormat GetRenderEngineFormat(cuttlefish::Image::Format format)
{
    switch (format)
    {
        case cuttlefish::Image::Format::Gray8:
            return TextureInternalFormat::R8;
        case cuttlefish::Image::Format::RGBA8:
            return TextureInternalFormat::RGBA8;
        default:
            return TextureInternalFormat::INVALID;
    }
}

cuttlefish::Texture::Format GetCuttlefishTextureFormat(cuttlefish::Image::Format format)
{
    switch (format)
    {
        case cuttlefish::Image::Format::Gray8:
            return cuttlefish::Texture::Format::R8;
        case cuttlefish::Image::Format::RGBA8:
            return cuttlefish::Texture::Format::R8G8B8A8;
        default:
            return cuttlefish::Texture::Format::Unknown;
    }
}

void CompileFont(const std::filesystem::path& path, const std::filesystem::path& outputPath)
{
    std::ifstream file(path, std::ios::binary);
    if (!file)
    {
        Debug::LogErrorFormat("No input file: {}", path.string());
        return;
    }

    std::vector<uint8_t> buffer((std::istreambuf_iterator<char>(file)),std::istreambuf_iterator<char>());

    const uint32_t version = static_cast<uint32_t>(buffer[3]);
    if (buffer[0] != 'B' || buffer[1] != 'M' || buffer[2] != 'F' || version != 3)
    {
        Debug::LogErrorFormat("Incorrect BMFont file identifier or version: {}", path.string());
        return;
    }

    CommonBlock commonBlock{};
    std::vector<const char*> pageNames;
    std::vector<Char> chars;
    std::vector<KerningPair> kerningPairs;

    uint32_t offset = 4;
    while (offset < buffer.size())
    {
        const BlockType blockType = ReadData<BlockType>(buffer.data(), offset);
        const uint32_t blockSize = ReadData<uint32_t>(buffer.data(), offset);

        switch (blockType)
        {
            case COMMON:
                commonBlock = ReadCommonBlock(&buffer[offset]);
                break;
            case PAGES:
                ReadPagesBlock(&buffer[offset], commonBlock.Pages, pageNames);
                break;
            case CHARS:
                ReadCharBlock(&buffer[offset], blockSize, chars);
                break;
            case KERNING_PAIRS:
                ReadKerningBlock(&buffer[offset], blockSize, kerningPairs);
                break;
        }

        offset += blockSize;
    }

    commonBlock.CharsCount = chars.size();
    commonBlock.KerningPairsCount = kerningPairs.size();

    std::vector<cuttlefish::Image*> pageImages;
    pageImages.resize(pageNames.size());

    for (int i = 0; i < pageNames.size(); ++i)
    {
        pageImages[i] = new cuttlefish::Image();

        std::string pagePath = (path.parent_path() / pageNames[i]).string();
        if (!pageImages[i]->load(pagePath.c_str(), cuttlefish::ColorSpace::Linear))
        {
            Debug::LogErrorFormat("Cannot load page image: {}", pagePath);
            return;
        }
    }

    TextureHeader header{};
    header.Depth = commonBlock.Pages;
    header.Width = commonBlock.ScaleW;
    header.Height = commonBlock.ScaleH;
    header.MipCount = 1;
    header.TextureFormat = GetRenderEngineFormat(pageImages[0]->format());
    header.IsLinear = true;

    if (header.TextureFormat == TextureInternalFormat::INVALID)
    {
        Debug::LogError("Invalid image format");
        return;
    }

    std::filesystem::create_directories(outputPath.parent_path());

    std::ofstream fout;
    fout.open(outputPath, std::ios::binary | std::ios::out);
    fout.write(reinterpret_cast<char*>(&commonBlock), sizeof(CommonBlock));
    fout.write(reinterpret_cast<char*>(&chars[0]), chars.size() * sizeof(Char));
    if (!kerningPairs.empty())
        fout.write(reinterpret_cast<char*>(&kerningPairs[0]), kerningPairs.size() * sizeof(KerningPair));
    fout.write(reinterpret_cast<char*>(&header), sizeof(TextureHeader));

    for (int i = 0; i < pageNames.size(); ++i)
    {
        cuttlefish::Image* image = pageImages[i];
        cuttlefish::Texture* texture = new cuttlefish::Texture(cuttlefish::Texture::Dimension::Dim2D, header.Width, header.Height, 0, 1, image->colorSpace());
        texture->setImage(*image);

        if (!texture->convert(GetCuttlefishTextureFormat(image->format()), cuttlefish::Texture::Type::UNorm))
        {
            Debug::LogError("Cannot load page image");
            return;
        }

        uint32_t imageSize = texture->dataSize();
        fout.write(reinterpret_cast<char*>(&imageSize), sizeof(uint32_t));
        fout.write(reinterpret_cast<const char*>(texture->data()), imageSize);
    }

    fout.close();

    Debug::LogInfoFormat("Font compiled: {}", path.string());
}

int main(int argc, char** argv)
{
    Arguments::Init(argv, argc);

    if (!Arguments::Contains("-input"))
        Debug::LogError("No -input argument");
    if (!Arguments::Contains("-output"))
        Debug::LogError("No -output argument");

    std::filesystem::path inputPath = Arguments::Get("-input");
    std::filesystem::path outputPath = Arguments::Get("-output");

    for (const std::filesystem::directory_entry& entry: std::filesystem::recursive_directory_iterator(inputPath))
    {
        const std::filesystem::path& path = entry.path();

        if (entry.is_regular_file() && path.extension() == ".fnt")
        {
            std::filesystem::path relativePath = std::filesystem::relative(path, inputPath);
            std::filesystem::path outputFontPath = outputPath / relativePath.parent_path() / relativePath.stem();

            CompileFont(path, outputFontPath);
        }
    }
}