#include <string>
#include <iostream>

#include "graphics_backend_api.h"
#include "texture_compressor_backend.h"
#include "texture_compressor_formats.h"

#include <GLFW/glfw3.h>

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

    if (!glfwInit())
    {
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, OPENGL_MAJOR_VERSION);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, OPENGL_MINOR_VERSION);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1, 1, "RenderEngine", nullptr, nullptr);
    if (!window)
    {
        return 1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    GraphicsBackend::Init(nullptr, "OpenGL");

    auto textureType = static_cast<TextureType>(std::stoi(argv[1]));
    int colorType = std::stoi(argv[2]);
    auto textureFormat = static_cast<TextureInternalFormat>(std::stoi(argv[3]));
    bool generateMips = std::stoi(argv[4]) == 1;

    std::vector<std::string> texturePaths;
    for (int i = 5; i < argc; ++i)
    {
        texturePaths.emplace_back(argv[i]);
    }

    while (!glfwWindowShouldClose(window))
    {
        TextureCompressorBackend::CompressTexture(texturePaths, textureType, colorType, textureFormat, generateMips);
        break;
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}