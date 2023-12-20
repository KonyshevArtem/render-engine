#include <string>
#include <GLUT/glut.h>
#include <iostream>

#include "texture_compressor_backend.h"
#include "texture_compressor_formats.h"
#include "game_window.h"
#include "debug.h"

std::string texturePath;
GLuint internalFormat;
int colorType;
bool generateMips;

void Render()
{
    TextureCompressorBackend::CompressTexture(texturePath, colorType, internalFormat, generateMips);
    exit(0);
}

void PrintHelp()
{
    std::cout << "Parameters: <texture path STRING> <input color type INT> <compressed format INT> <generate mipmaps BOOL>\n";

    std::cout << "\nAvailable input color types:\n";
    for (const auto &pair: TextureCompressorFormats::GetInputFormats())
    {
        std::cout << "\t" << pair.first << " - " << pair.second << "\n";
    }

    std::cout << "\nAvailable compressed types:\n";
    for (const auto &pair: TextureCompressorFormats::GetCompressedFormats())
    {
        std::cout << "\t" << pair.first << " - " << pair.second << "\n";
    }

    std::cout << std::endl;
}

int main(int __argc, char **__argv)
{
    if (__argc < 5)
    {
        PrintHelp();
        return 0;
    }

    Debug::Init();

    GameWindow window(0, 0, nullptr, Render, nullptr, nullptr);

    texturePath = std::string(__argv[1]);
    colorType = std::stoi(__argv[2]);
    internalFormat = std::stoi(__argv[3]);
    generateMips = std::stoi(__argv[4]) == 1;

    glutMainLoop();

    return 0;
}