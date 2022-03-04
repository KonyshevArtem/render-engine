#include "editor_window.h"
#include "hierarchy_gui/hierarchy_gui.h"
#include "ui_editor_window.h"
#include <QWidget>

EditorWindow::EditorWindow() :
    QMainWindow(nullptr),
    ui(new Ui::EditorWindow),
    m_Hierarchy(new HierarchyGUI())
{
    ui->setupUi(this);
    ui->root->addWidget(m_Hierarchy, 0, 0, -1, 1);

    setWindowTitle("Editor");
}

EditorWindow::~EditorWindow()
{
    delete ui;
}
