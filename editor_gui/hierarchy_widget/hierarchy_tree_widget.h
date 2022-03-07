#ifndef OPENGL_STUDY_HIERARCHYTREEWIDGET_H
#define OPENGL_STUDY_HIERARCHYTREEWIDGET_H

#include <QTreeWidget>
#include <memory>

class QFocusEvent;
class QDropEvent;
class GameObject;
class HierarchyTreeWidgetItem;

class HierarchyTreeWidget: public QTreeWidget
{
    Q_OBJECT

public:
    HierarchyTreeWidget();

protected:
    void focusInEvent(QFocusEvent *_event);
    void dropEvent(QDropEvent *_event);

private:
    void                     Update();
    HierarchyTreeWidgetItem *CollectHierarchy(const std::shared_ptr<GameObject> &_gameObject);
};

#endif
