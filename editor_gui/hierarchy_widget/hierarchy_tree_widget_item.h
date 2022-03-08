#ifndef HIERARCHY_TREE_WIDGET_ITEM_H
#define HIERARCHY_TREE_WIDGET_ITEM_H

#include <QTreeWidgetItem>
#include <memory>

class GameObject;

class HierarchyTreeWidgetItem: public QTreeWidgetItem
{
public:
    HierarchyTreeWidgetItem(const std::shared_ptr<GameObject> &_gameObject);

    std::shared_ptr<GameObject> GetGameObject() const;

private:
    std::weak_ptr<GameObject> m_GameObject;
};

#endif // HIERARCHY_TREE_WIDGET_ITEM_H
