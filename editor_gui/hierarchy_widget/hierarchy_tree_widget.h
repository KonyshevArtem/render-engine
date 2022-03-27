#ifndef OPENGL_STUDY_HIERARCHYTREEWIDGET_H
#define OPENGL_STUDY_HIERARCHYTREEWIDGET_H

#include <QTreeWidget>
#include <memory>
#include <unordered_map>

class QFocusEvent;
class QDropEvent;
class QPaintEvent;
class QKeyEvent;
class QPoint;
class QLineEdit;
class GameObject;
class HierarchyTreeWidgetItem;

class HierarchyTreeWidget: public QTreeWidget
{
    Q_OBJECT

public:
    HierarchyTreeWidget();

protected:
    void focusInEvent(QFocusEvent *_event);
    void dropEvent(QDropEvent *_event);
    void paintEvent(QPaintEvent *_event);
    void keyPressEvent(QKeyEvent *_event);

private:
    struct ItemStatus
    {
        bool Expanded;
        bool Selected;
        bool Current;
    };

    void Update();

    void TraverseHierarchy(
            HierarchyTreeWidgetItem                              *_widget,
            const std::function<void(HierarchyTreeWidgetItem *)> &_callback);

    void CollectStatusInfo(
            std::unordered_map<GameObject *, ItemStatus> &_status,
            HierarchyTreeWidgetItem                      *_widget);

    void RestoreStatus(
            const std::unordered_map<GameObject *, ItemStatus> &_status,
            HierarchyTreeWidgetItem                            *_widget);

    void CreateHierarchy(HierarchyTreeWidgetItem *_widget);
    void DestroyGameObjects(const std::vector<std::shared_ptr<GameObject>> &_gameObjects);

    bool IsRenaming() const;
    void StartRenaming(std::shared_ptr<GameObject> _gameObject);
    void StopRenaming(bool _success);

    std::vector<std::shared_ptr<GameObject>> GetSelectedGameObjects();

    QLineEdit                                                  *m_LineEdit;
    std::shared_ptr<GameObject>                                 m_RenamingGameObject;
    std::unordered_map<GameObject *, HierarchyTreeWidgetItem *> m_GameObjectToItem;

public slots:
    void PrepareMenu(const QPoint &_pos);
    void RenamingFinished();
};

#endif
