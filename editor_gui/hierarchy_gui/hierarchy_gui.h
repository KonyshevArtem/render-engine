#ifndef OPENGL_STUDY_HIERARCHYGUI_H
#define OPENGL_STUDY_HIERARCHYGUI_H

#include <QTreeWidget>
#include <memory>

class QLayout;
class QFocusEvent;
class GameObject;

class HierarchyGUI: public QTreeWidget
{
public:
    HierarchyGUI();

protected:
    void focusInEvent(QFocusEvent *event);

private:
    void             Update();
    QTreeWidgetItem *CollectHierarchy(const std::shared_ptr<GameObject> &_gameObject);
};

#endif // HIERARCHYGUI_H
