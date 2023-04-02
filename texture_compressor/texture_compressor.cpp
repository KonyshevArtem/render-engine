#include <string>
#include <QApplication>
#include <QCoreApplication>

#include "../core/texture/texture_header.h"
#include "texture_compressor_backend.h"
#include "texture_compressor_settings.h"
#include "game_window.h"
#include "lodepng.h"

bool interactiveMode;
std::string texturePath;
GLuint internalFormat;
LodePNGColorType colorType;

void Init()
{
    if (interactiveMode)
    {
        TextureCompressorBackend::InitInteractiveMode();
    }
}

void Resize(int width, int height)
{
}

void Render()
{
    if (interactiveMode)
    {
        TextureCompressorBackend::RenderInteractiveMode();
    }
    else
    {
        TextureCompressorBackend::CompressTexture(texturePath, colorType, internalFormat);
        QCoreApplication::quit();
    }
}

void OnKeyboardPressed(unsigned char key, bool down)
{
}

void OnMouseMove(double x, double y)
{
}

int main(int __argc, char** __argv)
{
    interactiveMode = __argc < 4;

    QApplication application(__argc, __argv);

    GameWindow window(Init, Resize, Render, OnKeyboardPressed, OnMouseMove);

    TextureCompressorSettings* settings;
    if (interactiveMode)
    {
        settings = new TextureCompressorSettings(&window);
        settings->show();
    }
    else
    {
        texturePath    = std::string(__argv[1]);
        colorType      = static_cast<LodePNGColorType>(std::stoi(__argv[2]));
        internalFormat = std::stoi(__argv[3]);
    }

    window.show();
    application.exec();

    if (settings)
    {
        delete settings;
    }

    return 0;
}