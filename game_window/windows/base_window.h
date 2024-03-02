#ifndef OPENGL_STUDY_BASE_WINDOW_H
#define OPENGL_STUDY_BASE_WINDOW_H

#include <string>
#include <memory>
#include <type_traits>

class BaseWindow;

template<class T>
concept DerivesBaseWindow = std::is_base_of_v<BaseWindow, T>;

class BaseWindow
{
public:
    static void DrawAllWindows();

    template<DerivesBaseWindow TWindow>
    static void Create()
    {
        AddWindow(std::make_shared<TWindow>());
    }

protected:
    BaseWindow(float width, float height, const std::string &title, size_t typeHashCode);
    virtual ~BaseWindow() = default;

    virtual void DrawInternal() = 0;

    int m_Id;

private:
    void Draw();

    size_t m_TypeHashCode;

    std::string m_Title;
    bool m_IsOpened;
    float m_InitialWidth;
    float m_InitialHeight;

    static void AddWindow(const std::shared_ptr<BaseWindow> &window);
};

#endif //OPENGL_STUDY_BASE_WINDOW_H
