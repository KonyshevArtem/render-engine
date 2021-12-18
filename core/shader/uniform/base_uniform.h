#ifndef OPENGL_STUDY_BASE_UNIFORM_H
#define OPENGL_STUDY_BASE_UNIFORM_H

#pragma clang diagnostic push
#pragma ide diagnostic   ignored "OCUnusedMacroInspection"
#define GL_SILENCE_DEPRECATION
#pragma clang diagnostic pop

#include "../uniform_type/uniform_type.h"
#include <OpenGL/gl3.h>
#include <memory>

using namespace std;

class BaseUniform
{
#pragma region fields

private:
    UniformType m_Type;
    GLint       m_Location;
    int         m_Index;

#pragma endregion

#pragma region construction

public:
    BaseUniform(GLint _location, GLenum _type, int _index);

private:
    BaseUniform(const BaseUniform &) = delete;

#pragma endregion

#pragma region public methods

public:
    void                      Set(const void *_value) const;
    [[nodiscard]] int         GetIndex() const;
    [[nodiscard]] UniformType GetType() const;

#pragma endregion
};

#endif //OPENGL_STUDY_BASE_UNIFORM_H
