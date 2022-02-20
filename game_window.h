#ifndef OPENGL_STUDY_WINDOW
#define OPENGL_STUDY_WINDOW

#include <QOpenGLWindow>
#include <functional>

class QKeyEvent;
class QMouseEvent;

typedef std::function<void(unsigned char, bool)> KeyboardInputHandlerDelegate;
typedef std::function<void(double, double)>      MouseMoveHandlerDelegate;

class GameWindow: public QOpenGLWindow
{
public:
    GameWindow(int                           _majorVersion,
               int                           _minorVersion,
               std::function<void()>         _init,
               std::function<void(int, int)> _resize,
               std::function<void()>         _render,
               KeyboardInputHandlerDelegate  _keyboardInputHandler,
               MouseMoveHandlerDelegate      _mouseMoveHandler);
    virtual ~GameWindow() = default;

protected:
    void initializeGL();
    void resizeGL(int _width, int _height);
    void paintGL();

    void keyPressEvent(QKeyEvent *_event);
    void keyReleaseEvent(QKeyEvent *_event);
    void mouseMoveEvent(QMouseEvent *_event);

private:
    std::function<void()>         m_Init;
    std::function<void(int, int)> m_Resize;
    std::function<void()>         m_Render;
    KeyboardInputHandlerDelegate  m_KeyboardInputHandler;
    MouseMoveHandlerDelegate      m_MouseMoveHandler;
};

#endif