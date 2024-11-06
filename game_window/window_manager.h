#ifndef RENDER_ENGINE_WINDOW_MANAGER_H
#define RENDER_ENGINE_WINDOW_MANAGER_H

#ifdef ENABLE_IMGUI

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

#endif

#endif //RENDER_ENGINE_WINDOW_MANAGER_H
