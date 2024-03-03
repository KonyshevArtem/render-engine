#include "window_manager.h"
#include "windows/base_window.h"

#include <vector>

std::vector<std::shared_ptr<BaseWindow>> s_Windows;

void WindowManager::DrawAllWindows()
{
    for (auto it = s_Windows.begin(); it != s_Windows.end();)
    {
        auto window = it->get();
        window->Draw();

        if (window->IsOpened())
        {
            ++it;
        }
        else
        {
            it = s_Windows.erase(it);
        }
    }
}

void WindowManager::AddWindow(const std::shared_ptr<BaseWindow> &window)
{
    s_Windows.push_back(window);
}