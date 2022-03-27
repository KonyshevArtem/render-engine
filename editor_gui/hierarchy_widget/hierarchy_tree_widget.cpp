#include "hierarchy_tree_widget.h"
#include "editor/hierarchy.h"
#include "gameObject/gameObject.h"
#include "hierarchy_tree_widget_item.h"
#include "scene/scene.h"
#include <QDropEvent>
#include <QFocusEvent>
#include <QKeyEvent>
#include <QLineEdit>
#include <QMenu>
#include <QPaintEvent>
#include <QPoint>
#include <QSignalMapper>


#ifdef OPENGL_STUDY_WINDOWS
#define OPENGL_STUDY_RENAME_KEY Qt::Key_F2
#elif OPENGL_STUDY_MACOS
#define OPENGL_STUDY_RENAME_KEY Qt::Key_Enter
#endif


HierarchyTreeWidget::HierarchyTreeWidget() :
    QTreeWidget(nullptr),
    m_LineEdit(new QLineEdit(this)),
    m_RenamingGameObject(nullptr)
{
    setHeaderHidden(true);
    setIndentation(10);
    setDragEnabled(true);
    setDragDropMode(DragDropMode::DragDrop);
    setSelectionMode(SelectionMode::ExtendedSelection);
    setContextMenuPolicy(Qt::CustomContextMenu);

    connect(this, &QTreeWidget::customContextMenuRequested, this, &HierarchyTreeWidget::PrepareMenu);

    m_LineEdit->setVisible(false);
    connect(m_LineEdit, &QLineEdit::editingFinished, this, &HierarchyTreeWidget::RenamingFinished);
}

void HierarchyTreeWidget::focusInEvent(QFocusEvent *_event)
{
    Update();
    QTreeWidget::focusInEvent(_event);
}

void HierarchyTreeWidget::dropEvent(QDropEvent *_event)
{
    auto droppedGOs = GetSelectedGameObjects();
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

void HierarchyTreeWidget::paintEvent(QPaintEvent *_event)
{
    QTreeWidget::paintEvent(_event);

    Hierarchy::SetSelectedGameObjects(GetSelectedGameObjects());
}

void HierarchyTreeWidget::keyPressEvent(QKeyEvent *_event)
{
    QTreeWidget::keyPressEvent(_event);

    auto key = _event->key();
    if (IsRenaming())
    {
        if (key == Qt::Key_Escape)
            StopRenaming(false);
    }
    else
    {
        if (key == Qt::Key_Delete)
        {
            auto selectedGameObjects = GetSelectedGameObjects();
            DestroyGameObjects(selectedGameObjects);
        }
        else if (key == OPENGL_STUDY_RENAME_KEY)
        {
            auto selectedGameObjects = GetSelectedGameObjects();
            if (!selectedGameObjects.empty())
                StartRenaming(selectedGameObjects[0]);
        }
    }
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

    m_GameObjectToItem.clear();

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
        auto widget                   = new HierarchyTreeWidgetItem(*it);
        m_GameObjectToItem[it->get()] = widget;

        if (haveWidget)
            _widget->addChild(widget);
        else
            addTopLevelItem(widget);

        CreateHierarchy(widget);
    }
}

void HierarchyTreeWidget::PrepareMenu(const QPoint &_pos)
{
    auto selectedGameObjects = GetSelectedGameObjects();
    if (selectedGameObjects.empty())
        return;

    auto renameAction = new QAction("Rename", this);
    connect(renameAction, &QAction::triggered, this, [this, &selectedGameObjects]
            { StartRenaming(selectedGameObjects[0]); });

    auto deleteAction = new QAction("Delete", this);
    connect(deleteAction, &QAction::triggered, this, [this, &selectedGameObjects]
            { DestroyGameObjects(selectedGameObjects); });

    QMenu menu(this);
    menu.addAction(renameAction);
    menu.addAction(deleteAction);
    menu.exec(mapToGlobal(_pos));
}

void HierarchyTreeWidget::DestroyGameObjects(const std::vector<std::shared_ptr<GameObject>> &_gameObjects)
{
    if (_gameObjects.empty())
        return;

    for (const auto &go: _gameObjects)
        go->Destroy();

    Update();
}

std::vector<std::shared_ptr<GameObject>> HierarchyTreeWidget::GetSelectedGameObjects()
{
    auto                                     items = selectedItems();
    std::vector<std::shared_ptr<GameObject>> selectedGameObjects;
    for (auto it = items.cbegin(); it != items.cend(); it++)
    {
        auto widget     = reinterpret_cast<HierarchyTreeWidgetItem *>(*it);
        auto gameObject = widget ? widget->GetGameObject() : nullptr;
        if (gameObject)
            selectedGameObjects.push_back(gameObject);
    }
    return selectedGameObjects;
}

bool HierarchyTreeWidget::IsRenaming() const
{
    return m_LineEdit->isVisible() && m_RenamingGameObject;
}

void HierarchyTreeWidget::StartRenaming(std::shared_ptr<GameObject> _gameObject)
{
    if (!_gameObject || !m_GameObjectToItem.contains(_gameObject.get()))
        return;

    auto item = m_GameObjectToItem.at(_gameObject.get());
    if (!item)
        return;

    auto rect = visualItemRect(item);
    m_LineEdit->move(rect.topLeft());
    m_LineEdit->resize(rect.size());

    m_LineEdit->setText(_gameObject->Name.c_str());
    m_LineEdit->selectAll();

    m_LineEdit->setVisible(true);
    m_LineEdit->setFocus();

    m_RenamingGameObject = _gameObject;
}

void HierarchyTreeWidget::StopRenaming(bool _success)
{
    if (_success && m_RenamingGameObject)
    {
        auto newName               = m_LineEdit->text();
        m_RenamingGameObject->Name = newName.toStdString();

        if (m_GameObjectToItem.contains(m_RenamingGameObject.get()))
            m_GameObjectToItem.at(m_RenamingGameObject.get())->setText(0, newName);
    }

    // important to set gameobject to nullptr before disabling line edit
    m_RenamingGameObject = nullptr;
    m_LineEdit->setVisible(false);
}

void HierarchyTreeWidget::RenamingFinished()
{
    StopRenaming(true);
}