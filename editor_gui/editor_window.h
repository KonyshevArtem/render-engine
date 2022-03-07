#ifndef EDITOR_WINDOW_H
#define EDITOR_WINDOW_H

#include <QMainWindow>

class HierarchyWidget;

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
    Ui::EditorWindow *m_UI;

    HierarchyWidget *m_Hierarchy;
};

#endif // EDITOR_WINDOW_H
