#include "hierarchy_tree_widget_item.h"
#include "gameObject/gameObject.h"

HierarchyTreeWidgetItem::HierarchyTreeWidgetItem(const std::shared_ptr<GameObject> &_gameObject) :
    QTreeWidgetItem({_gameObject ? _gameObject->Name.c_str() : "GameObject"}),
    m_GameObject(_gameObject)
{
}

std::shared_ptr<GameObject> HierarchyTreeWidgetItem::GetGameObject()
{
    return !m_GameObject.expired() ? m_GameObject.lock() : nullptr;
}