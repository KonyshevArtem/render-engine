#ifndef OPENGL_STUDY_RENDER_PASS_H
#define OPENGL_STUDY_RENDER_PASS_H

#include "memory"
#include "unordered_map"

class Shader;
class Material;
class Context;

using namespace std;

class RenderPass
{
public:
    RenderPass();

    void Execute(shared_ptr<Context> &_ctx);

private:
    void BindDefaultTextures(const shared_ptr<Shader> &_shader, unordered_map<string, int> &_textureUnits);
    void TransferUniformsFromMaterial(const shared_ptr<Material> &_material, const unordered_map<string, int> &_textureUnits);
};

#endif //OPENGL_STUDY_RENDER_PASS_H
