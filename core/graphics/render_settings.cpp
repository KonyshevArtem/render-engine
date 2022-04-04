#include "render_settings.h"

bool RenderSettings::TagsMatch(const Shader &_shader, int _passIndex) const
{
    for (const auto &pair: Tags)
    {
        if (_shader.GetPassTagValue(_passIndex, pair.first) != pair.second)
            return false;
    }
    return true;
}