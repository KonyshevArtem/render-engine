#include "hierarchy_widget.h"
#include "gameObject/gameObject.h"
#include "scene/scene.h"
#include "ui_hierarchy_widget.h"
#include <QEvent>
#include <QFocusEvent>
#include <QLayout>
#include <QStringList>
#include <QTreeWidgetItem>
#include <QWidget>

HierarchyWidget::HierarchyWidget() :
    QWidget(),
    m_UI(new Ui::HierarchyWidget)
{
    m_UI->setupUi(this);

    m_Tree = m_UI->tree;
    m_Tree->installEventFilter(this);
}

HierarchyWidget::~HierarchyWidget()
{
    delete m_UI;
}

bool HierarchyWidget::eventFilter(QObject *_Object, QEvent *_Event)
{
    if (_Object == m_Tree && _Event->type() == QEvent::FocusIn)
        Update();
    return false;
}

void HierarchyWidget::Update()
{
    if (!Scene::Current)
        return;

    m_Tree->clear();

    for (auto it = Scene::Current->cbegin(); it != Scene::Current->cend(); it++)
    {
        auto widget = CollectHierarchy(*it);
        if (widget)
            m_Tree->addTopLevelItem(widget);
    }
}

QTreeWidgetItem *HierarchyWidget::CollectHierarchy(const std::shared_ptr<GameObject> &_gameObject)
{
    if (!_gameObject)
        return nullptr;

    auto widget = new QTreeWidgetItem({_gameObject->Name.c_str()});
    for (auto it = _gameObject->Children.cbegin(); it != _gameObject->Children.cend(); it++)
    {
        auto childWidget = CollectHierarchy(*it);
        if (widget)
            widget->addChild(childWidget);
    }

    return widget;
}
