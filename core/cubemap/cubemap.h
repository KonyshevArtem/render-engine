#ifndef OPENGL_STUDY_CUBEMAP_H
#define OPENGL_STUDY_CUBEMAP_H

#include "../texture/texture.h"
#include <OpenGL/gl3.h>
#include <filesystem>
#include <memory>
#include <vector>

using namespace std;

class Cubemap: public Texture
{
public:
    static shared_ptr<Cubemap> Load(const filesystem::path &_xPositivePath,
                                    const filesystem::path &_xNegativePath,
                                    const filesystem::path &_yPositivePath,
                                    const filesystem::path &_yNegativePath,
                                    const filesystem::path &_zPositivePath,
                                    const filesystem::path &_zNegativePath);

    void Bind(int _unit) const override;

private:
    void Init();

    vector<vector<unsigned char>> m_Data;
};

#endif