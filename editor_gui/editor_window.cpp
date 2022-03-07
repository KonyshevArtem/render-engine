#include "editor_window.h"
#include "hierarchy_widget/hierarchy_widget.h"
#include "ui_editor_window.h"
#include <QWidget>

EditorWindow::EditorWindow() :
    QMainWindow(nullptr),
    m_UI(new Ui::EditorWindow),
    m_Hierarchy(new HierarchyWidget())
{
    m_UI->setupUi(this);
    m_UI->root->addWidget(m_Hierarchy, 0, 0, -1, 1);

    setWindowTitle("Editor");
}

EditorWindow::~EditorWindow()
{
    delete m_UI;
}
