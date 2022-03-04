#include "hierarchy_gui.h"
#include "gameObject/gameObject.h"
#include "scene/scene.h"
#include <QFocusEvent>
#include <QLayout>
#include <QStringList>
#include <QTreeWidgetItem>

HierarchyGUI::HierarchyGUI() :
    QTreeWidget()
{
    setSizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Preferred);
    setMaximumWidth(250);
    setHeaderHidden(true);
}

void HierarchyGUI::focusInEvent(QFocusEvent *event)
{
    Update();
}

void HierarchyGUI::Update()
{
    if (!Scene::Current)
        return;

    clear();

    for (const auto &go: Scene::Current->GameObjects)
    {
        addTopLevelItem(new QTreeWidgetItem(QStringList {go->Name.c_str()}));
    }
}
