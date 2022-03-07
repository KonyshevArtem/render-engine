#include "hierarchy_tree_widget.h"
#include "gameObject/gameObject.h"
#include "hierarchy_tree_widget_item.h"
#include "scene/scene.h"
#include <QDropEvent>
#include <QFocusEvent>

HierarchyTreeWidget::HierarchyTreeWidget() :
    QTreeWidget(nullptr)
{
    setHeaderHidden(true);
    setIndentation(10);
    setDragEnabled(true);
    setDragDropMode(DragDropMode::DragDrop);
}

void HierarchyTreeWidget::focusInEvent(QFocusEvent *_event)
{
    Update();
    QTreeWidget::focusInEvent(_event);
}

void HierarchyTreeWidget::dropEvent(QDropEvent *_event)
{
    auto droppedWidget = currentItem();
    auto dropped       = reinterpret_cast<HierarchyTreeWidgetItem *>(droppedWidget);
    if (!dropped || dropped->m_GameObject.expired())
        return;

    auto dropIndicator = dropIndicatorPosition();
    if (dropIndicator == DropIndicatorPosition::OnItem)
    {
        auto targetWidget = itemFromIndex(indexAt(_event->position().toPoint()));
        auto target       = reinterpret_cast<HierarchyTreeWidgetItem *>(targetWidget);
        auto newParent    = target && !target->m_GameObject.expired() ? target->m_GameObject.lock() : nullptr;
        dropped->m_GameObject.lock()->SetParent(newParent);
    }
    else
        dropped->m_GameObject.lock()->SetParent(nullptr);

    Update();
}

void HierarchyTreeWidget::Update()
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

HierarchyTreeWidgetItem *HierarchyTreeWidget::CollectHierarchy(const std::shared_ptr<GameObject> &_gameObject)
{
    if (!_gameObject)
        return nullptr;

    auto widget = new HierarchyTreeWidgetItem(_gameObject);
    for (auto it = _gameObject->Children.cbegin(); it != _gameObject->Children.cend(); it++)
    {
        auto childWidget = CollectHierarchy(*it);
        if (childWidget)
            widget->addChild(childWidget);
    }

    return widget;
}
