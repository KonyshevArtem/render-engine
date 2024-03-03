#ifndef OPENGL_STUDY_WINDOW_MANAGER_H
#define OPENGL_STUDY_WINDOW_MANAGER_H

#include <memory>
#include <type_traits>

#define DECLARE_STATIC_CONSTRUCTOR inline static class _init { public: _init(); } init
#define DEFINE_STATIC_CONSTRUCTOR(type) type::_init::_init()

class BaseWindow;

namespace WindowManager
{
    template<class T>
    concept DerivesBaseWindow = std::is_base_of_v<BaseWindow, T>;

    void DrawAllWindows();
    void AddWindow(const std::shared_ptr<BaseWindow> &window);

    template<DerivesBaseWindow TWindow>
    void Create()
    {
        AddWindow(std::make_shared<TWindow>());
    }
}

#endif //OPENGL_STUDY_WINDOW_MANAGER_H
