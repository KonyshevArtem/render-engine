#include "game_window.h"

#include <utility>
#include "GLUT/glut.h"

GameWindow *k_GameWindow = nullptr;

GameWindow::GameWindow(int width,
                       int height,
                       std::function<void(int, int)> _resize,
                       std::function<void()> _render,
                       KeyboardInputHandlerDelegate _keyboardInputHandler,
                       MouseMoveHandlerDelegate _mouseMoveHandler) :
        m_Resize(std::move(_resize)),
        m_Render(std::move(_render)),
        m_KeyboardInputHandler(std::move(_keyboardInputHandler)),
        m_MouseMoveHandler(std::move(_mouseMoveHandler))
{

    int argc = 0;
    glutInit(&argc, nullptr);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_3_2_CORE_PROFILE);
    glutInitWindowSize(width, height);
    glutCreateWindow("GameWindow");

    glutKeyboardFunc(keyPressEvent);
    glutKeyboardUpFunc(keyReleaseEvent);
    glutMotionFunc(mouseMoveEvent);
    glutPassiveMotionFunc(mouseMoveEvent);

    glutDisplayFunc(paintGL);
    glutReshapeFunc(resizeGL);

    k_GameWindow = this;
}

void GameWindow::resizeGL(int _width, int _height)
{
    if (k_GameWindow->m_Resize)
    {
        k_GameWindow->m_Resize(_width, _height);
    }
}

void GameWindow::paintGL()
{
    if (k_GameWindow->m_Render)
    {
        k_GameWindow->m_Render();
    }

    glutSwapBuffers();
    glutPostRedisplay();
}

void GameWindow::keyPressEvent(unsigned char key, int x, int y)
{
    if (k_GameWindow->m_KeyboardInputHandler)
    {
        k_GameWindow->m_KeyboardInputHandler(static_cast<char>(key), true);
    }
}

void GameWindow::keyReleaseEvent(unsigned char key, int x, int y)
{
    if (k_GameWindow->m_KeyboardInputHandler)
    {
        k_GameWindow->m_KeyboardInputHandler(static_cast<char>(key), false);
    }
}

void GameWindow::mouseMoveEvent(int x, int y)
{
    if (k_GameWindow->m_MouseMoveHandler)
    {
        k_GameWindow->m_MouseMoveHandler(x, y);
    }
}