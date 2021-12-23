#ifndef OPENGL_STUDY_RENDER_PASS_H
#define OPENGL_STUDY_RENDER_PASS_H

#include <memory>
#include <unordered_map>

class Context;

using namespace std;

class RenderPass
{
public:
    RenderPass();

    void Execute(const Context &_ctx);

private:
    RenderPass(const RenderPass &) = delete;
};

#endif //OPENGL_STUDY_RENDER_PASS_H
