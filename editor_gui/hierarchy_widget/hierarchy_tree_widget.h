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
    void Update();

    void TraverseHierarchy(
            HierarchyTreeWidgetItem                              *_widget,
            const std::function<void(HierarchyTreeWidgetItem *)> &_callback);

    void CollectExpandedStatus(
            std::unordered_map<GameObject *, bool> &_outExpandedStatus,
            HierarchyTreeWidgetItem                *_widget) const;

    void RestoreExpandedStatus(
            const std::unordered_map<GameObject *, bool> &_expandedStatus,
            HierarchyTreeWidgetItem                      *_widget);

    void CreateHierarchy(HierarchyTreeWidgetItem *_widget);
};

#endif
