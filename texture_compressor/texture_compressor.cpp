#include <string>
#include <iostream>

#include "graphics_backend_api.h"
#include "texture_compressor_backend.h"
#include "texture_compressor_formats.h"
#include "game_window.h"

TextureType textureType;
std::vector<std::string> texturePaths;
TextureInternalFormat textureFormat;
int colorType;
bool generateMips;

void Render(int width, int height)
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

int main(int argc, char **argv)
{
    if (argc < 6)
    {
        PrintHelp();
        return 0;
    }

    GameWindow window(1, 1, Render, nullptr, nullptr);

    GraphicsBackend::Current()->Init();

    textureType = static_cast<TextureType>(std::stoi(argv[1]));
    colorType = std::stoi(argv[2]);
    textureFormat = static_cast<TextureInternalFormat>(std::stoi(argv[3]));
    generateMips = std::stoi(argv[4]) == 1;

    for (int i = 5; i < argc; ++i)
    {
        texturePaths.emplace_back(argv[i]);
    }

    window.BeginMainLoop();

    return 0;
}