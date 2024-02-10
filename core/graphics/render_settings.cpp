#include "render_settings.h"
#include "shader/shader_pass/shader_pass.h"

bool RenderSettings::TagsMatch(const ShaderPass &_shaderPass) const
{
    for (const auto &pair: Tags)
    {
        if (_shaderPass.GetTagValue(pair.first) != pair.second)
            return false;
    }
    return true;
}