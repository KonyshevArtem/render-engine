#ifndef EDITOR_WINDOW_H
#define EDITOR_WINDOW_H

#include <QMainWindow>

class HierarchyGUI;

namespace Ui
{
    class EditorWindow;
}

class EditorWindow: public QMainWindow
{
    Q_OBJECT

public:
    EditorWindow();
    ~EditorWindow();

private:
    Ui::EditorWindow *ui;

    HierarchyGUI *m_Hierarchy;
};

#endif // EDITOR_WINDOW_H
