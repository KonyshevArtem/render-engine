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
    auto dropped   = reinterpret_cast<HierarchyTreeWidgetItem *>(currentItem());
    auto droppedGO = dropped ? dropped->GetGameObject() : nullptr;
    if (!droppedGO)
        return;

    auto index    = indexAt(_event->position().toPoint());
    auto target   = reinterpret_cast<HierarchyTreeWidgetItem *>(itemFromIndex(index));
    auto targetGO = target ? target->GetGameObject() : nullptr;

    if (targetGO)
    {
        auto indicator = dropIndicatorPosition();
        if (indicator == DropIndicatorPosition::OnItem)
            droppedGO->SetParent(targetGO);
        else
        {
            int row = index.row() + (indicator == DropIndicatorPosition::BelowItem ? 1 : 0);
            droppedGO->SetParent(targetGO->GetParent(), row);
        }
    }
    else
        droppedGO->SetParent(nullptr);

    _event->setDropAction(Qt::DropAction::MoveAction);
    QTreeWidget::dropEvent(_event);
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
