#include <string>
#include <iostream>

#include "texture_compressor_backend.h"
#include "texture_compressor_formats.h"
#include "game_window.h"

TextureType textureType;
std::vector<std::string> texturePaths;
TextureInternalFormat textureFormat;
int colorType;
bool generateMips;

void Render()
{
    TextureCompressorBackend::CompressTexture(texturePaths, textureType, colorType,
                                              textureFormat, generateMips);
    exit(0);
}

void PrintHelp()
{
    std::cout << "Parameters: <texture type INT> <input color type INT> <texture format INT> "
                 "<generate mipmaps BOOL> <texture paths STRING>\n";

    std::cout << "\nAvailable texture types:\n";
    for (const auto &typeInfo: TextureCompressorFormats::GetTextureTypesInfo())
    {
        std::cout << "\t" << typeInfo.Name << " - " << static_cast<int>(typeInfo.Type) << "\n";
    }

    std::cout << "\nAvailable input color types:\n";
    for (const auto &pair: TextureCompressorFormats::GetInputFormats())
    {
        std::cout << "\t" << pair.first << " - " << pair.second << "\n";
    }

    std::cout << "\nAvailable texture formats:\n";
    for (const auto &pair: TextureCompressorFormats::GetTextureFormats())
    {
        std::cout << "\t" << static_cast<int>(pair.first) << " - " << pair.second << "\n";
    }

    std::cout << std::endl;
}

int main(int __argc, char **__argv)
{
    if (__argc < 6)
    {
        PrintHelp();
        return 0;
    }

    GameWindow window(0, 0, nullptr, Render, nullptr, nullptr);

    textureType = static_cast<TextureType>(std::stoi(__argv[1]));
    colorType = std::stoi(__argv[2]);
    textureFormat = static_cast<TextureInternalFormat>(std::stoi(__argv[3]));
    generateMips = std::stoi(__argv[4]) == 1;

    for (int i = 5; i < __argc; ++i)
    {
        texturePaths.emplace_back(__argv[i]);
    }

    window.BeginMainLoop();

    return 0;
}