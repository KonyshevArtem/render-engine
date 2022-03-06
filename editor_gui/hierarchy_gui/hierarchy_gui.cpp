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

    for (auto it = Scene::Current->cbegin(); it != Scene::Current->cend(); it++)
    {
        auto widget = CollectHierarchy(*it);
        if (widget)
            addTopLevelItem(widget);
    }
}

QTreeWidgetItem *HierarchyGUI::CollectHierarchy(const std::shared_ptr<GameObject> &_gameObject)
{
    if (!_gameObject)
        return nullptr;

    auto widget = new QTreeWidgetItem({_gameObject->Name.c_str()});
    for (auto it = _gameObject->Children.cbegin(); it != _gameObject->Children.cend(); it++)
    {
        auto childWidget = CollectHierarchy(*it);
        widget->addChild(childWidget);
    }

    return widget;
}
