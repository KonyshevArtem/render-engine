#include "shader_loader.h"
#include "../utils/utils.h"
#include "string"
#include <cstdio>
#include <cstdlib>

GLuint ShaderLoader::LoadShader(GLenum shaderType, const std::string& path)
{
    char *shaderSource = Utils::ReadFile(path);

    GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &shaderSource, nullptr);

    glCompileShader(shader);

    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE)
    {
        GLint infoLogLength;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);

        auto *logMsg = new GLchar[infoLogLength + 1];
        glGetShaderInfoLog(shader, infoLogLength, nullptr, logMsg);

        fprintf(stderr, "Shader compilation failed: %s\n%s\n", path.c_str(), logMsg);

        free(logMsg);
    }
    else
        printf("Shader compile success: %s\n", path.c_str());

    free(shaderSource);

    return shader;
}
