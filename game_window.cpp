#include "game_window.h"
#include <QKeyEvent>
#include <QMouseEvent>

GameWindow::GameWindow(int                           _majorVersion,
                       int                           _minorVersion,
                       std::function<void()>         _init,
                       std::function<void(int, int)> _resize,
                       std::function<void()>         _render,
                       KeyboardInputHandlerDelegate  _keyboardInputHandler,
                       MouseMoveHandlerDelegate      _mouseMoveHandler) :
    QOpenGLWindow(),
    m_Init(_init),
    m_Resize(_resize),
    m_Render(_render),
    m_KeyboardInputHandler(_keyboardInputHandler),
    m_MouseMoveHandler(_mouseMoveHandler)
{
    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setMajorVersion(_majorVersion);
    format.setMinorVersion(_minorVersion);
    format.setProfile(QSurfaceFormat::OpenGLContextProfile::CoreProfile);
    format.setSwapBehavior(QSurfaceFormat::SwapBehavior::DoubleBuffer);
    setFormat(format);
}

void GameWindow::initializeGL()
{
    m_Init();
}

void GameWindow::resizeGL(int _width, int _height)
{
    static auto pixelRatio = devicePixelRatio();
    m_Resize(_width * pixelRatio, _height * pixelRatio);
}

void GameWindow::paintGL()
{
    m_Render();
    update();
}

void GameWindow::keyPressEvent(QKeyEvent *_event)
{
    m_KeyboardInputHandler(static_cast<char>(_event->key()), true);
}

void GameWindow::keyReleaseEvent(QKeyEvent *_event)
{
    m_KeyboardInputHandler(static_cast<char>(_event->key()), false);
}

void GameWindow::mouseMoveEvent(QMouseEvent *_event)
{
    auto pos = _event->localPos();
    m_MouseMoveHandler(pos.x(), pos.y());
}