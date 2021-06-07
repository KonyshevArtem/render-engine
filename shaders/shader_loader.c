#include "shader_loader.h"
#include "../utils/utils.h"
#include <stdio.h>
#include <stdlib.h>

GLuint loadShader(GLenum shaderType, char *path)
{
    char *shaderSource = readFile(path);

    GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &shaderSource, NULL);

    glCompileShader(shader);

    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE)
    {
        GLint infoLogLength;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);

        GLchar *logMsg = malloc(infoLogLength + 1);
        glGetShaderInfoLog(shader, infoLogLength, NULL, logMsg);

        fprintf(stderr, "Shader compilation failed: %s\n%s\n", path, logMsg);

        free(logMsg);
    }
    else
        printf("Shader compile success: %s\n", path);

    free(shaderSource);

    return shader;
}