#include "arguments.h"

#include "../core/texture/texture_header.h"
#include "cuttlefish/Image.h"
#include "debug.h"

#include <iostream>
#include <fstream>
#include <filesystem>
#include <cstdint>

enum BlockType : uint8_t
{
    INFO = 1,
    COMMON,
    PAGES,
    CHARS,
    KERNING_PAIRS
};

struct CommonBlock
{
    uint16_t LineHeight;
    uint16_t Base;
    uint16_t ScaleW;
    uint16_t ScaleH;
    uint16_t Pages;
    uint8_t BitField;
    uint8_t AlphaChannel;
    uint8_t RedChannel;
    uint8_t GreenChannel;
    uint8_t BlueChannel;
    uint16_t CharsCount;
    uint16_t KerningPairsCount;
};

struct Char
{
    uint32_t Id;
    uint16_t X;
    uint16_t Y;
    uint16_t Width;
    uint16_t Height;
    int16_t XOffset;
    int16_t YOffset;
    int16_t XAdvance;
    uint8_t Page;
    uint8_t Channel;
};

struct KerningPair
{
    uint32_t First;
    uint32_t Second;
    int16_t Amount;
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
        if (!pageImages[i]->load(pagePath.c_str(), cuttlefish::ColorSpace::sRGB))
        {
            Debug::LogErrorFormat("Cannot load page image: {}", pagePath);
            return;
        }
    }

    std::filesystem::create_directories(outputPath.parent_path());

    std::ofstream fout;
    fout.open(outputPath, std::ios::binary | std::ios::out);
    fout.write(reinterpret_cast<char*>(&commonBlock), sizeof(CommonBlock));
    fout.write(reinterpret_cast<char*>(&chars[0]), chars.size() * sizeof(Char));
    if (!kerningPairs.empty())
        fout.write(reinterpret_cast<char*>(&kerningPairs[0]), kerningPairs.size() * sizeof(KerningPair));

    for (int i = 0; i < pageNames.size(); ++i)
    {
        cuttlefish::Image* image = pageImages[i];

        std::vector<uint8_t> imageData;
        image->save(imageData, "tga");

        TextureHeader header{};
        header.Depth = 1;
        header.Width = image->width();
        header.Height = image->height();
        header.MipCount = 1;
        header.TextureFormat = TextureInternalFormat::RGBA8;
        header.IsLinear = true;

        uint32_t imageSize = imageData.size();
        fout.write(reinterpret_cast<char*>(&header), sizeof(TextureHeader));
        fout.write(reinterpret_cast<char*>(&imageSize), sizeof(uint32_t));
        fout.write(reinterpret_cast<char*>(&imageData[0]), imageSize);
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