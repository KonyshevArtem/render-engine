#include "hierarchy_widget.h"
#include "hierarchy_tree_widget.h"
#include "ui_hierarchy_widget.h"

HierarchyWidget::HierarchyWidget() :
    QWidget(),
    m_UI(new Ui::HierarchyWidget),
    m_Tree(new HierarchyTreeWidget)
{
    m_UI->setupUi(this);
    m_UI->frame->layout()->addWidget(m_Tree);
}

HierarchyWidget::~HierarchyWidget()
{
    delete m_Tree;
    delete m_UI;
}
