#ifndef OPENGL_STUDY_HIERARCHYWIDGET_H
#define OPENGL_STUDY_HIERARCHYWIDGET_H

#include <QWidget>
#include <memory>

class HierarchyTreeWidget;

namespace Ui
{
    class HierarchyWidget;
}

class HierarchyWidget: public QWidget
{
    Q_OBJECT

public:
    HierarchyWidget();
    ~HierarchyWidget();

private:
    Ui::HierarchyWidget *m_UI;
    HierarchyTreeWidget *m_Tree;
};

#endif // HIERARCHYWIDGET_H
