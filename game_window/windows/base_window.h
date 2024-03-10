#ifndef RENDER_ENGINE_BASE_WINDOW_H
#define RENDER_ENGINE_BASE_WINDOW_H

#include <string>

class BaseWindow
{
public:
    void Draw();

    inline bool IsOpened() const
    {
        return m_IsOpened;
    }

protected:
    BaseWindow(float width, float height, const std::string &title, size_t typeHashCode, bool topBar = true);
    virtual ~BaseWindow();

    virtual void DrawInternal() = 0;

    int m_Id;

private:
    size_t m_TypeHashCode;

    std::string m_Title;
    bool m_IsOpened;
    float m_InitialWidth;
    float m_InitialHeight;
    bool m_TopBar;
};

#endif //RENDER_ENGINE_BASE_WINDOW_H
