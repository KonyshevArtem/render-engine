#include <string>
#include <GLUT/glut.h>
#include <iostream>

#include "texture_compressor_backend.h"
#include "game_window.h"
#include "lodepng.h"
#include "debug.h"

std::string texturePath;
GLuint internalFormat;
LodePNGColorType colorType;

void Render()
{
    TextureCompressorBackend::CompressTexture(texturePath, colorType, internalFormat);
    exit(0);
}

void printHelp()
{
    auto &colorFormats = TextureCompressorBackend::GetInputFormats();
    auto &compressedFormat = TextureCompressorBackend::GetCompressedFormats();

    std::cout << "Parameters: <texture path STRING> <input color type INT> <compressed format INT>\n";

    std::cout << "\nAvailable input color types:\n";
    for (const auto &pair: colorFormats)
    {
        std::cout << "\t" << pair.first << " - " << pair.second << "\n";
    }

    std::cout << "\nAvailable compressed types:\n";
    for (const auto &pair: compressedFormat)
    {
        std::cout << "\t" << pair.first << " - " << pair.second << "\n";
    }

    std::cout << std::endl;
}

int main(int __argc, char **__argv)
{
    if (__argc < 4)
    {
        printHelp();
        return 0;
    }

    Debug::Init();

    GameWindow window(0, 0, nullptr, Render, nullptr, nullptr);

    texturePath = std::string(__argv[1]);
    colorType = static_cast<LodePNGColorType>(std::stoi(__argv[2]));
    internalFormat = std::stoi(__argv[3]);

    glutMainLoop();

    return 0;
}