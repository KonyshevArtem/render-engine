#ifndef OPENGL_STUDY_HIERARCHYGUI_H
#define OPENGL_STUDY_HIERARCHYGUI_H

#include <QTreeWidget>

class QLayout;
class QFocusEvent;

class HierarchyGUI: public QTreeWidget
{
public:
    HierarchyGUI();

protected:
    void focusInEvent(QFocusEvent *event);

private:
    void Update();
};

#endif // HIERARCHYGUI_H
