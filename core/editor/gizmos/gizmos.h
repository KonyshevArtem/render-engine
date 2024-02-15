#if OPENGL_STUDY_EDITOR

#ifndef OPENGL_STUDY_GIZMOS_H
#define OPENGL_STUDY_GIZMOS_H

#include <vector>

struct Matrix4x4;
struct DrawCallInfo;

namespace Gizmos
{
    void DrawWireCube(const Matrix4x4 &_matrix);

    void Init();
    const std::vector<DrawCallInfo> &GetDrawInfos();
    void ClearDrawInfos();
} // namespace Gizmos

#endif

#endif