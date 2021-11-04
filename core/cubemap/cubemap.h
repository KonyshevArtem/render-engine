#ifndef OPENGL_STUDY_CUBEMAP_H
#define OPENGL_STUDY_CUBEMAP_H

#include <OpenGL/gl3.h>
#include <filesystem>
#include <memory>
#include <vector>

using namespace std;

class Cubemap
{
public:
    static shared_ptr<Cubemap> Load(const filesystem::path &_xPositivePath,
                                    const filesystem::path &_xNegativePath,
                                    const filesystem::path &_yPositivePath,
                                    const filesystem::path &_yNegativePath,
                                    const filesystem::path &_zPositivePath,
                                    const filesystem::path &_zNegativePath);

    unsigned int Width;
    unsigned int Height;

    void Bind(int _unit) const;

private:
    void Init();

    GLuint                        m_Texture;
    GLuint                        m_Sampler;
    vector<vector<unsigned char>> m_Data;
};

#endif