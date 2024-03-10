#ifndef OPENGL_STUDY_TOP_MENU_BAR_H
#define OPENGL_STUDY_TOP_MENU_BAR_H

#include <functional>

namespace TopMenuBar
{
    void Draw(std::function<void()> closeWindow);
}

#endif //OPENGL_STUDY_TOP_MENU_BAR_H
