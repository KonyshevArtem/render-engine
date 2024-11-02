#include <string>
#include <iostream>

#include "texture_compressor_backend.h"
#include "texture_compressor_formats.h"

void PrintHelp()
{
    std::cout << "Parameters: <texture type STRING> <texture format STRING> <is linear BOOL>"
                 "<generate mipmaps BOOL> <output name STRING> <texture paths STRING>\n";

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
    if (argc < 7)
    {
        PrintHelp();
        return 0;
    }

    std::string textureType = argv[1];
    std::string textureFormat = argv[2];
    bool isLinear = std::stoi(argv[3]) == 1;
    bool generateMips = std::stoi(argv[4]) == 1;
    std::string outputName = argv[5];

    std::vector<std::string> texturePaths;
    for (int i = 6; i < argc; ++i)
    {
        texturePaths.emplace_back(argv[i]);
    }

    TextureCompressorBackend::CompressTexture(texturePaths, textureType, textureFormat, isLinear, generateMips, outputName);

    return 0;
}