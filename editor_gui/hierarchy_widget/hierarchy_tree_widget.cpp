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
    setSelectionMode(SelectionMode::ContiguousSelection);
}

void HierarchyTreeWidget::focusInEvent(QFocusEvent *_event)
{
    Update();
    QTreeWidget::focusInEvent(_event);
}

void HierarchyTreeWidget::dropEvent(QDropEvent *_event)
{
    auto                                     droppedItems = selectedItems();
    std::vector<std::shared_ptr<GameObject>> droppedGOs;
    for (auto it = droppedItems.cbegin(); it != droppedItems.cend(); it++)
    {
        auto widget     = reinterpret_cast<HierarchyTreeWidgetItem *>(*it);
        auto gameObject = widget ? widget->GetGameObject() : nullptr;
        if (gameObject)
            droppedGOs.push_back(gameObject);
    }

    if (droppedGOs.size() == 0)
        return;

    auto index    = indexAt(_event->position().toPoint());
    auto target   = reinterpret_cast<HierarchyTreeWidgetItem *>(itemFromIndex(index));
    auto targetGO = target ? target->GetGameObject() : nullptr;

    for (const auto &gameObject: droppedGOs)
    {
        if (targetGO)
        {
            auto indicator = dropIndicatorPosition();
            if (indicator == DropIndicatorPosition::OnItem)
                gameObject->SetParent(targetGO);
            else
            {
                int row = index.row() + (indicator == DropIndicatorPosition::BelowItem ? 1 : 0);
                gameObject->SetParent(targetGO->GetParent(), row);
            }
        }
        else
            gameObject->SetParent(nullptr);
    }

    _event->setDropAction(Qt::DropAction::MoveAction);
    QTreeWidget::dropEvent(_event);
}

static void CollectExpandedStatus(
        std::unordered_map<GameObject *, bool> &_outExpandedStatus,
        HierarchyTreeWidgetItem                *_widget)
{
    auto go = _widget ? _widget->GetGameObject() : nullptr;
    if (go)
        _outExpandedStatus[go.get()] = _widget->isExpanded();
}

static void RestoreExpandedStatus(
        const std::unordered_map<GameObject *, bool> &_expandedStatus,
        HierarchyTreeWidgetItem                      *_widget)
{
    auto go = _widget ? _widget->GetGameObject() : nullptr;
    if (!go)
        return;

    auto goPtr = go.get();
    if (_expandedStatus.contains(goPtr))
        _widget->setExpanded(_expandedStatus.at(goPtr));
}

void HierarchyTreeWidget::Update()
{
    if (!Scene::Current)
        return;

    std::unordered_map<GameObject *, bool> expanded;
    TraverseHierarchy(nullptr, [&expanded](HierarchyTreeWidgetItem *_widget)
                      { CollectExpandedStatus(expanded, _widget); });

    clear();
    CreateHierarchy(nullptr);

    TraverseHierarchy(nullptr, [&expanded](HierarchyTreeWidgetItem *_widget)
                      { RestoreExpandedStatus(expanded, _widget); });
}

void HierarchyTreeWidget::TraverseHierarchy(
        HierarchyTreeWidgetItem                              *_widget,
        const std::function<void(HierarchyTreeWidgetItem *)> &_callback)
{
    bool haveWidget = _widget != nullptr;
    auto itemsCount = haveWidget ? _widget->childCount() : topLevelItemCount();

    for (int i = 0; i < itemsCount; ++i)
    {
        auto child = reinterpret_cast<HierarchyTreeWidgetItem *>(haveWidget ? _widget->child(i) : topLevelItem(i));
        if (!child)
            continue;

        _callback(child);

        TraverseHierarchy(child, _callback);
    }
}

void HierarchyTreeWidget::CreateHierarchy(HierarchyTreeWidgetItem *_widget)
{
    auto haveWidget = _widget != nullptr;
    auto parent     = haveWidget ? _widget->GetGameObject() : nullptr;
    auto begin      = parent ? parent->Children.cbegin() : Scene::Current->cbegin();
    auto end        = parent ? parent->Children.cend() : Scene::Current->cend();

    for (auto &it = begin; it != end; it++)
    {
        auto widget = new HierarchyTreeWidgetItem(*it);

        if (haveWidget)
            _widget->addChild(widget);
        else
            addTopLevelItem(widget);

        CreateHierarchy(widget);
    }
}
