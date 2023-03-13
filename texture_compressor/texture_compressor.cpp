#include <iostream>
#include <filesystem>
#include <vector>
#include <fstream>
#ifdef TEXTURE_COMPRESSOR_WINDOWS
#include <GL/glew.h>
#elif TEXTURE_COMPRESSOR_MACOS
#include <OpenGL/gl3.h>
#include <OpenGL/glext.h>
#endif
#include <QApplication>
#include <QCoreApplication>

#include "../core/texture/texture_header.h"
#include "game_window.h"
#include "lodepng.h"

enum ColorType
{
    RGB = 0,
    RGBA = 1,
    GREY = 2,
};

std::filesystem::path texturePath;
GLuint internalFormat;
GLuint format;
ColorType colorType;

void CompressTexture()
{
    constexpr int headerSize = sizeof(TextureHeader);

    LodePNGColorType lodepngColorType = LCT_RGB;
    switch (colorType)
    {
        case RGBA:
            lodepngColorType = LCT_RGBA;
            break;
        case GREY:
            lodepngColorType = LCT_GREY;
            break;
        default:
            lodepngColorType = LCT_RGB;
            break;
    }

    TextureHeader header;
    std::vector<unsigned char> pixels;

    lodepng::decode(pixels, header.Width, header.Height, texturePath.string(), lodepngColorType);

    header.Format = format;

    GLuint texture = 0;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, header.Width, header.Height, 0, header.Format, GL_UNSIGNED_BYTE, pixels.data());

    int isCompressed;
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_COMPRESSED, &isCompressed);

    if (isCompressed){
        int compressedSize;
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_COMPRESSED_IMAGE_SIZE, &compressedSize);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &header.InternalFormat);

        int totalSize = headerSize + compressedSize;
        unsigned char* compressedPixels = new unsigned char[totalSize];
        glGetCompressedTexImage(GL_TEXTURE_2D, 0, compressedPixels + headerSize);

        memcpy(compressedPixels, reinterpret_cast<void*>(&header), headerSize);

        std::ofstream fout;
        auto outputPath = texturePath.replace_extension("");
        fout.open(outputPath, std::ios::binary | std::ios::out);
        fout.write(reinterpret_cast<char*>(compressedPixels), totalSize * sizeof(char));
        fout.close();

        delete[] compressedPixels;

        std::cout << "Texture successfuly compressed. Format: " << header.InternalFormat << ", Original Size: " << pixels.size() * sizeof(char) << ", Compresed Size: " << compressedSize << std::endl;
    }
    else{
        std::cout << "Failed to compress texture" << std::endl;
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    glDeleteTextures(1, &texture);

    QCoreApplication::quit();
}

int main(int __argc, char** __argv)
{
    if (__argc < 5){
        std::cout << "Not all parameters provided" << std::endl;
        return 0;
    }

    texturePath = std::filesystem::absolute(__argv[1]);
    colorType = static_cast<ColorType>(std::stoi(__argv[2]));
    internalFormat = std::stoi(__argv[3]);
    format = std::stoi(__argv[4]);

    QApplication application(__argc, __argv);

    GameWindow window([]() -> void {}, [](int, int) -> void {}, CompressTexture, nullptr, nullptr);
    window.show();

    application.exec();

    return 0;
}