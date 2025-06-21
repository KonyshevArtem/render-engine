#include <string>
#include <iostream>

#include "texture_compressor_backend.h"
#include "texture_compressor_formats.h"
#include "arguments.h"
#include "string_split.h"

void PrintHelp()
{
    std::cout << "Parameters: -type <texture type STRING> -format <texture format STRING> -linear <is linear BOOL>"
                 "-mips <generate mipmaps BOOL> -output <output path STRING> -inputs <texture paths STRING>\n";

    std::cout << "\nAvailable texture types:\n";
    for (const auto &typeInfo: TextureCompressorFormats::GetTextureTypesInfo())
    {
        std::cout << "\t" << typeInfo.Name << "\n";
    }

    std::cout << "\nAvailable texture formats:\n";
    for (const auto &formatInfo: TextureCompressorFormats::GetTextureFormatsInfo())
    {
        std::cout << "\t" << formatInfo.Name << "\n";
    }

    std::cout << std::endl;
}

int main(int argc, char **argv)
{
    Arguments::Init(argv, argc);

    if (!Arguments::Contains("-type") ||
        !Arguments::Contains("-format") ||
        !Arguments::Contains("-output") ||
        !Arguments::Contains("-inputs"))
    {
        PrintHelp();
        return 0;
    }

    std::string textureType = Arguments::Get("-type");
    std::string textureFormat = Arguments::Get("-format");
    bool isLinear = Arguments::Contains("-linear");
    bool generateMips = Arguments::Contains("-mips");
    std::string outputPath = Arguments::Get("-output");
    std::vector<std::string> texturePaths = StringSplit::Split(Arguments::Get("-inputs"), ',');

    TextureCompressorBackend::CompressTexture(texturePaths, textureType, textureFormat, isLinear, generateMips, outputPath);

    return 0;
}