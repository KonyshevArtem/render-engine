#include "texture_2d.h"
#include "core_debug/debug.h"
#include "lodepng.h"
#include "utils.h"
#ifdef OPENGL_STUDY_WINDOWS
#include <GL/glew.h>
#elif OPENGL_STUDY_MACOS
#include <OpenGL/gl3.h>
#endif

std::shared_ptr<Texture2D> Texture2D::Create(int _width, int _height)
{
    auto t      = std::shared_ptr<Texture2D>(new Texture2D());
    t->m_Width  = _width;
    t->m_Height = _height;

    t->Init(GL_SRGB_ALPHA, GL_RGBA, GL_UNSIGNED_BYTE, false);

    return t;
}

std::shared_ptr<Texture2D> Texture2D::CreateShadowMap(int _width, int _height)
{
    auto t      = std::shared_ptr<Texture2D>(new Texture2D());
    t->m_Width  = _width;
    t->m_Height = _height;

    t->Init(GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, false);

    return t;
}

std::shared_ptr<Texture2D> Texture2D::Load(const std::filesystem::path &_path, bool _srgb, bool _hasAlpha)
{
    auto t = std::shared_ptr<Texture2D>(new Texture2D());

    auto colorType = _hasAlpha ? LCT_RGBA : LCT_RGB;
    auto error     = lodepng::decode(t->m_Data, t->m_Width, t->m_Height, (Utils::GetExecutableDirectory() / _path).string(), colorType);
    if (error != 0)
    {
        Debug::LogErrorFormat("[Texture2D] Error loading texture: %1%", {lodepng_error_text(error)});
        return nullptr;
    }

    auto internalFormat = _srgb
                                  ? _hasAlpha
                                            ? GL_SRGB_ALPHA
                                            : GL_SRGB
                          : _hasAlpha
                                  ? GL_RGBA
                                  : GL_RGB;

    auto format = _hasAlpha ? GL_RGBA : GL_RGB;

    t->Init(internalFormat, format, GL_UNSIGNED_BYTE, true);

    return t;
}

const std::shared_ptr<Texture2D> &Texture2D::White()
{
    static std::shared_ptr<Texture2D> white;

    if (white != nullptr)
        return white;

    white           = std::shared_ptr<Texture2D>(new Texture2D());
    white->m_Width  = 1;
    white->m_Height = 1;

    white->m_Data.push_back(255);
    white->m_Data.push_back(255);
    white->m_Data.push_back(255);

    white->Init(GL_SRGB, GL_RGB, GL_UNSIGNED_BYTE, true);

    return white;
}

const std::shared_ptr<Texture2D> &Texture2D::Normal()
{
    static std::shared_ptr<Texture2D> normal;

    if (normal != nullptr)
        return normal;

    normal           = std::shared_ptr<Texture2D>(new Texture2D());
    normal->m_Width  = 1;
    normal->m_Height = 1;

    normal->m_Data.push_back(125);
    normal->m_Data.push_back(125);
    normal->m_Data.push_back(255);

    normal->Init(GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, true);

    return normal;
}

const std::shared_ptr<Texture2D> &Texture2D::Null()
{
    static std::shared_ptr<Texture2D> null;
    return null;
}

void Texture2D::Init(GLint _internalFormat, GLenum _format, GLenum _type, bool _loadData)
{
    CHECK_GL(glGenTextures(1, &m_Texture));
    CHECK_GL(glGenSamplers(1, &m_Sampler));
    CHECK_GL(glBindTexture(GL_TEXTURE_2D, m_Texture));
    CHECK_GL(glSamplerParameteri(m_Sampler, GL_TEXTURE_WRAP_S, GL_REPEAT));
    CHECK_GL(glSamplerParameteri(m_Sampler, GL_TEXTURE_WRAP_T, GL_REPEAT));
    CHECK_GL(glSamplerParameteri(m_Sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    CHECK_GL(glSamplerParameteri(m_Sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    CHECK_GL(glTexImage2D(GL_TEXTURE_2D, 0, _internalFormat, m_Width, m_Height, 0, _format, _type, _loadData ? m_Data.data() : nullptr)); // NOLINT(cppcoreguidelines-narrowing-conversions)
    CHECK_GL(glBindTexture(GL_TEXTURE_2D, 0));
}

void Texture2D::Bind(int _unit) const
{
    CHECK_GL(glActiveTexture(GL_TEXTURE0 + _unit));
    CHECK_GL(glBindTexture(GL_TEXTURE_2D, m_Texture));
    CHECK_GL(glBindSampler(_unit, m_Sampler));
}

void Texture2D::Attach(GLenum _attachment)
{
    CHECK_GL(glFramebufferTexture(GL_FRAMEBUFFER, _attachment, m_Texture, 0));
}

void Texture2D::SetWrapMode(GLenum _wrapMode) const
{
    CHECK_GL(glBindTexture(GL_TEXTURE_2D, m_Texture));
    CHECK_GL(glSamplerParameteri(m_Sampler, GL_TEXTURE_WRAP_S, _wrapMode));
    CHECK_GL(glSamplerParameteri(m_Sampler, GL_TEXTURE_WRAP_T, _wrapMode));
    CHECK_GL(glBindTexture(GL_TEXTURE_2D, 0));
}

void Texture2D::SetBorderColor(const Vector4 &_color) const
{
    CHECK_GL(glBindTexture(GL_TEXTURE_2D, m_Texture));
    CHECK_GL(glSamplerParameterfv(m_Sampler, GL_TEXTURE_BORDER_COLOR, &_color.x));
    CHECK_GL(glBindTexture(GL_TEXTURE_2D, 0));
}
