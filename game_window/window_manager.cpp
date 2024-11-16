#ifdef ENABLE_IMGUI

#include "window_manager.h"
#include "windows/base_window.h"

#include <vector>

std::vector<std::shared_ptr<BaseWindow>> s_Windows;

void WindowManager::DrawAllWindows()
{
    for (int i = 0; i < s_Windows.size(); ++i)
    {
        auto window = s_Windows[i];
        if (window->IsOpened())
        {
            window->Draw();
        }
        else
        {
            s_Windows.erase(s_Windows.begin() + i);
            --i;
        }
    }
}

void WindowManager::AddWindow(const std::shared_ptr<BaseWindow> &window)
{
    s_Windows.push_back(window);
}

#endif