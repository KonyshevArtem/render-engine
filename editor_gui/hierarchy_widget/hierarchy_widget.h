#ifndef OPENGL_STUDY_HIERARCHYWIDGET_H
#define OPENGL_STUDY_HIERARCHYWIDGET_H

#include <QWidget>
#include <memory>

class QLayout;
class QFocusEvent;
class QTreeWidget;
class QTreeWidgetItem;
class GameObject;

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

protected:
    bool eventFilter(QObject *object, QEvent *event);

private:
    Ui::HierarchyWidget *m_UI;
    QTreeWidget         *m_Tree;

    void             Update();
    QTreeWidgetItem *CollectHierarchy(const std::shared_ptr<GameObject> &_gameObject);
};

#endif // HIERARCHYWIDGET_H
