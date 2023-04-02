#include "texture_compressor_backend.h"
#ifdef TEXTURE_COMPRESSOR_WINDOWS
#include <GL/glew.h>
#elif TEXTURE_COMPRESSOR_MACOS
#include <OpenGL/gl3.h>
#include <OpenGL/glu.h>
#endif
#include <iostream>
#include <vector>
#include <filesystem>
#include <fstream>
#include <unordered_map>

#include "../core/texture/texture_header.h"
#include "lodepng.h"

namespace TextureCompressorBackend
{
    GLuint m_VertexArrayBuffer;
    GLuint m_VAO;
    GLuint m_PreviewShaderProgram;
    GLuint m_BackgroundShaderProgram;

    GLuint m_Texture = 0;
    GLuint m_Sampler;

    std::vector<std::pair<int, std::string>> m_InputFormats =
    {
        {LCT_RGB, "RGB"},
        {LCT_RGBA, "RGBA"},
        {LCT_GREY, "GREY"},
        {LCT_GREY_ALPHA, "GREY ALPHA"},
    };

    std::unordered_map<int, int> m_ColorTypeToFormat = 
    {
        {LCT_RGB, GL_RGB},
        {LCT_RGBA, GL_RGBA},
        {LCT_GREY, GL_RED},
        {LCT_GREY_ALPHA, GL_RG}
    };

    std::vector<std::pair<int, std::string>> m_CompressedFormats = 
    {
        {GL_RGB, "RGB"},
        {GL_RGBA, "RGBA"},
        {GL_SRGB, "sRGB"},
        {GL_SRGB_ALPHA, "sRGBA"},
        {GL_COMPRESSED_RGB, "Compressed RGB"},
        {GL_COMPRESSED_RGBA, "Compressed RGBA"},
        {GL_COMPRESSED_SRGB, "Compressed sRGB"},
        {GL_COMPRESSED_SRGB_ALPHA, "Compressed sRGBA"},

        #if GL_ARB_texture_rg
        {GL_COMPRESSED_RED, "Compressed Red"},
        {GL_COMPRESSED_RG, "Compressed RG"},
        #endif

        #if GL_EXT_texture_compression_s3tc
        {GL_COMPRESSED_RGB_S3TC_DXT1_EXT, "RGB S3TC DXT1"},
        {GL_COMPRESSED_RGBA_S3TC_DXT1_EXT, "RGBA S3TC DXT1"},
        {GL_COMPRESSED_RGBA_S3TC_DXT3_EXT, "RGBA S3TC DXT3"},
        {GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, "RGBA S3TC DXT5"},
        #endif

        #if GL_EXT_texture_sRGB
        {GL_COMPRESSED_SRGB_S3TC_DXT1_EXT, "SRGB S3TC DXT1"},
        {GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT, "sRGBA S3TC DXT1"},
        {GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT, "sRGBA S3TC DXT3"},
        {GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT, "sRGBA S3TC DXT5"},
        #endif
    };

    void InitInteractiveMode()
    {
        float vertices[] = {
            -1.0f, -1.0f, 0.5f,
            1.0f, -1.0f, 0.5f,
            1.0f,  1.0f, 0.5f,
            -1.0f, -1.0f, 0.5f,
            1.0f, 1.0f, 0.5f,
            -1.0f, 1.0f, 0.5f,
            0.0f, 0.0f,
            1.0f, 0.0f,
            1.0f, 1.0f,
            0.0f, 0.0f,
            1.0f, 1.0f,
            0.0f, 1.0f
        };

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glGenVertexArrays(1, &m_VAO);
        glBindVertexArray(m_VAO);

        glGenBuffers(1, &m_VertexArrayBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, m_VertexArrayBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<const void*>(3 * 6 * sizeof(float)));

        const char *vertexShaderSource   = "#version 410 core\n"
                                           "layout (location = 0) in vec3 aPos;\n"
                                           "layout (location = 1) in vec2 texcoord;\n"
                                           "out vec2 uv;\n"
                                           "void main()\n"
                                           "{\n"
                                           "   uv.x = texcoord.x;\n"
                                           "   uv.y = 1 - texcoord.y;\n"
                                           "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
                                           "}\0";
        const char *previewFragmentShaderSource = "#version 410 core\n"
                                           "in vec2 uv;\n"
                                           "uniform sampler2D _Tex;\n"
                                           "out vec4 FragColor;\n"
                                           "void main()\n"
                                           "{\n"
                                           "    FragColor = texture(_Tex, uv);\n"
                                           "}\0";
        const char *backgroundFragmentShaderSource = "#version 410 core\n"
                                           "in vec2 uv;\n"
                                           "out vec4 FragColor;\n"
                                           "void main()\n"
                                           "{\n"
                                           "    float chessboard = floor(uv.x * 10) + floor(uv.y * 10);\n"
                                           "    chessboard = fract(chessboard * 0.5) * 2;\n"
                                           "    FragColor = mix(vec4(1, 1, 1, 1), vec4(0.8, 0.8, 0.8, 0.8), chessboard);\n"
                                           "}\0";                                           

        GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
        GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

        glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
        glShaderSource(fragmentShader, 1, &previewFragmentShaderSource, nullptr);
        glCompileShader(vertexShader);
        glCompileShader(fragmentShader);

        m_PreviewShaderProgram = glCreateProgram();
        glAttachShader(m_PreviewShaderProgram, vertexShader);
        glAttachShader(m_PreviewShaderProgram, fragmentShader);
        glLinkProgram(m_PreviewShaderProgram);

        glShaderSource(fragmentShader, 1, &backgroundFragmentShaderSource, nullptr);
        glCompileShader(fragmentShader);
        
        m_BackgroundShaderProgram = glCreateProgram();
        glAttachShader(m_BackgroundShaderProgram, vertexShader);
        glAttachShader(m_BackgroundShaderProgram, fragmentShader);
        glLinkProgram(m_BackgroundShaderProgram);

        GLint success;
        glGetProgramiv(m_PreviewShaderProgram, GL_LINK_STATUS, &success);
        glGetProgramiv(m_BackgroundShaderProgram, GL_LINK_STATUS, &success);
        if (!success)
        {
            std::cout << "OpenGL shader compilation error" << std::endl;
        }

        GLenum error = glGetError();
        if (error != 0)
        {
            std::cout << "OpenGL error when initializing interactive mode: " << reinterpret_cast<const char *>(gluErrorString(error)) << std::endl;
        }
    }

    void RenderInteractiveMode()
    {
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindVertexArray(m_VAO);
        glUseProgram(m_BackgroundShaderProgram);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        if (m_Texture != 0)
        {
            glUseProgram(m_PreviewShaderProgram);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, m_Texture);
            glBindSampler(0, m_Sampler);

            glDrawArrays(GL_TRIANGLES, 0, 6);
        }

        GLenum error = glGetError();
        if (error != 0)
        {
            std::cout << "OpenGL error when rendering image: " << reinterpret_cast<const char *>(gluErrorString(error)) << std::endl;
        }
    }

    std::string GetFormatName(int format)
    {
        for (int i = 0; i < m_CompressedFormats.size(); ++i)
        {
            const auto& pair = m_CompressedFormats[i];
            if (pair.first == format)
            {
                return pair.second;
            }
        }

        return "Unknown";
    }

    TextureInfo LoadTexture(const std::string &path)
    {
        std::filesystem::path pngPath(path);
        std::filesystem::path compressedPath = pngPath.replace_extension("");

        if (m_Texture == 0)
        {
            glGenTextures(1, &m_Texture);
            glGenSamplers(1, &m_Sampler);
        }

        glBindTexture(GL_TEXTURE_2D, m_Texture);
        glSamplerParameteri(m_Sampler, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glSamplerParameteri(m_Sampler, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glSamplerParameteri(m_Sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glSamplerParameteri(m_Sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        TextureInfo info;
        if (std::filesystem::exists(compressedPath))
        {
            static constexpr int headerSize = sizeof(TextureHeader);

            std::ifstream input(compressedPath.string(), std::ios::binary);
            std::vector<char> pixels((std::istreambuf_iterator<char>(input)), (std::istreambuf_iterator<char>()));
            input.close();

            TextureHeader header = *reinterpret_cast<TextureHeader*>(pixels.data());
            glCompressedTexImage2D(GL_TEXTURE_2D, 0, header.InternalFormat, header.Width, header.Height, 0, sizeof(char) * pixels.size() - headerSize, pixels.data() + headerSize);

            info.Width = header.Width;
            info.Height = header.Height;
            info.Size = (pixels.size() * sizeof(char) - headerSize) / 1024;
            info.Format = GetFormatName(header.InternalFormat);
        }
        else
        {
            std::vector<unsigned char> pixels;

            lodepng::decode(pixels, info.Width, info.Height, path, LCT_RGB);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, info.Width, info.Height, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());

            info.Size = pixels.size() * sizeof(char) / 1024;
            info.Format = GetFormatName(GL_SRGB);
        }

        GLenum error = glGetError();
        if (error != 0)
        {
            std::cout << "OpenGL error when loading texture: " << reinterpret_cast<const char *>(gluErrorString(error)) << std::endl;
        }

        return info;
    }

    void CompressTexture(const std::string& path, int colorType, int compressedFormat)
    {
        constexpr int headerSize = sizeof(TextureHeader);

        TextureHeader header;
        std::vector<unsigned char> pixels;

        lodepng::decode(pixels, header.Width, header.Height, path, static_cast<LodePNGColorType>(colorType));

        header.Format = m_ColorTypeToFormat[colorType];

        std::cout << compressedFormat << std::endl;

        GLuint texture = 0;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, compressedFormat, header.Width, header.Height, 0, header.Format, GL_UNSIGNED_BYTE, pixels.data());

        GLenum error = glGetError();
        if (error != 0)
        {
            std::cout << "OpenGL error when initializing interactive mode: " << reinterpret_cast<const char *>(gluErrorString(error)) << std::endl;
        }

        int isCompressed;
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_COMPRESSED, &isCompressed);

        if (isCompressed)
        {
            int compressedSize;
            glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_COMPRESSED_IMAGE_SIZE, &compressedSize);
            glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &header.InternalFormat);

            int totalSize = headerSize + compressedSize;
            unsigned char* compressedPixels = new unsigned char[totalSize];
            glGetCompressedTexImage(GL_TEXTURE_2D, 0, compressedPixels + headerSize);

            memcpy(compressedPixels, reinterpret_cast<void*>(&header), headerSize);

            std::ofstream fout;
            auto outputPath = std::filesystem::path(path).replace_extension("");
            fout.open(outputPath, std::ios::binary | std::ios::out);
            fout.write(reinterpret_cast<char*>(compressedPixels), totalSize * sizeof(char));
            fout.close();

            delete[] compressedPixels;

            std::cout << "Texture successfuly compressed. Format: " << header.InternalFormat << ", Original Size: " << pixels.size() * sizeof(char) << ", Compresed Size: " << compressedSize << std::endl;
        }
        else
        {
            std::cout << "Failed to compress texture" << std::endl;
        }

        glBindTexture(GL_TEXTURE_2D, 0);
        glDeleteTextures(1, &texture);
    }

    const std::vector<std::pair<int, std::string>> &GetInputFormats()
    {
        return m_InputFormats;
    }

    const std::vector<std::pair<int, std::string>> &GetCompressedFormats()
    {
        return m_CompressedFormats;
    }
}