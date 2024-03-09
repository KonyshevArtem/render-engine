#include "scene_hierarchy_window.h"
#include "imgui.h"
#include "imgui_stdlib.h"
#include "scene/scene.h"
#include "editor/hierarchy.h"

const char *s_DragPayloadName = "DragAndDropPayload";
constexpr ImGuiTreeNodeFlags s_BaseFlags = ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow |
                                           ImGuiTreeNodeFlags_SpanFullWidth;

float s_LastItemY;
bool s_IsSelectingRange;
std::shared_ptr<GameObject> s_SelectingRangeTarget;
std::vector<std::weak_ptr<GameObject>> s_SelectedGameObjectsCopy;

bool s_ReparentRequest;
int s_ReparentIndex;
std::shared_ptr<GameObject> s_NewParent;

bool s_DestroyRequest;

SceneHierarchyWindow::SceneHierarchyWindow() :
        BaseWindow(600, 800, "Hierarchy", typeid(SceneHierarchyWindow).hash_code(), false)
{
}

ImGuiTreeNodeFlags GetFlags(bool isSelected, bool hasChildren)
{
    ImGuiTreeNodeFlags flags = s_BaseFlags;
    if (isSelected)
    {
        flags |= ImGuiTreeNodeFlags_Selected;
    }
    if (!hasChildren)
    {
        flags |= ImGuiTreeNodeFlags_Leaf;
    }

    return flags;
}

std::string GetLabel(const std::shared_ptr<GameObject> &go, bool isRenaming)
{
    auto name = isRenaming ? "" : go->Name;
    return name + "###" + std::to_string(go->GetUniqueID());
}

void HandleSelect(const std::shared_ptr<GameObject> &go, std::unordered_set<std::shared_ptr<GameObject>> &selectedGameObjects, bool selectAll, bool selectRange)
{
    bool isClicked = ImGui::IsItemHovered() && !ImGui::IsItemToggledOpen() && ImGui::IsMouseReleased(ImGuiMouseButton_Left);

    if (selectAll)
    {
        selectedGameObjects.insert(go);
    }
    else if (selectRange)
    {
        bool wasSelectingRange = s_IsSelectingRange;
        if (go == s_SelectingRangeTarget != isClicked)
        {
            s_IsSelectingRange = !s_IsSelectingRange;
        }

        if ((wasSelectingRange || s_IsSelectingRange) && !selectedGameObjects.contains(go))
        {
            selectedGameObjects.insert(go);
        }
    }
    else if (isClicked)
    {
        if (ImGui::GetIO().KeyCtrl)
        {
            if (selectedGameObjects.contains(go))
            {
                selectedGameObjects.erase(go);
            }
            else
            {
                selectedGameObjects.insert(go);
            }
        }
        else
        {
            selectedGameObjects.clear();
            selectedGameObjects.insert(go);
        }
    }
}

void CopySelectedGameObjects(const std::shared_ptr<GameObject> &go, std::unordered_set<std::shared_ptr<GameObject>> &selectedGameObjects)
{
    s_SelectedGameObjectsCopy.clear();
    if (selectedGameObjects.contains(go))
    {
        s_SelectedGameObjectsCopy.reserve(selectedGameObjects.size());
        for (auto &selectedGo : selectedGameObjects)
        {
            s_SelectedGameObjectsCopy.push_back(selectedGo);
        }
    }
    else
    {
        selectedGameObjects.clear();
        selectedGameObjects.insert(go);
        s_SelectedGameObjectsCopy.push_back(go);
    }
}

void HandleDrag(const std::shared_ptr<GameObject> &go, std::unordered_set<std::shared_ptr<GameObject>> &selectedGameObjects)
{
    if (ImGui::BeginDragDropSource())
    {
        CopySelectedGameObjects(go, selectedGameObjects);

        auto payloadText = s_SelectedGameObjectsCopy.size() > 1
                           ? "Multiple GameObjects (" + std::to_string(s_SelectedGameObjectsCopy.size()) + ")"
                           : go->Name;

        ImGui::SetDragDropPayload(s_DragPayloadName, nullptr, 0);
        ImGui::Text((payloadText).c_str());
        ImGui::EndDragDropSource();
    }
}

void HandleDrop(const std::shared_ptr<GameObject> &newParent, int reparentIndex = -1)
{
    if (ImGui::BeginDragDropTarget())
    {
        if (ImGui::AcceptDragDropPayload(s_DragPayloadName))
        {
            s_ReparentRequest = true;
            s_ReparentIndex = reparentIndex;
            s_NewParent = newParent;
        }
        ImGui::EndDragDropTarget();
    }
}

void HandleFreeDrop()
{
    auto max = ImGui::GetContentRegionMax();
    auto size = ImVec2{max.x, max.y - s_LastItemY};

    if (size.x > 0 && size.y > 0)
    {
        ImGui::SetCursorPos({0, s_LastItemY});
        ImGui::InvisibleButton("Free Drop", size);

        HandleDrop(nullptr);
    }
}

bool SceneHierarchyWindow::DrawRenameInput(std::shared_ptr<GameObject> &go)
{
    static std::string renameBuffer;

    if (!m_IsRenaming || go != m_RenameTarget)
    {
        return false;
    }

    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    if (ImGui::InputText("###RenameInput", &renameBuffer, ImGuiInputTextFlags_EnterReturnsTrue))
    {
        go->Name = renameBuffer;
    }

    if (m_RenamingNeedsFocus)
    {
        ImGui::SetKeyboardFocusHere(-1);
        renameBuffer = go->Name;
    }
    else if (!ImGui::IsItemActive())
    {
        m_IsRenaming = false;
    }

    m_RenamingNeedsFocus = false;
    ImGui::SameLine();

    return true;
}

void SceneHierarchyWindow::DrawContextMenu(std::shared_ptr<GameObject> &go, std::unordered_set<std::shared_ptr<GameObject>> &selectedGameObjects)
{
    if (ImGui::BeginPopupContextItem())
    {
        CopySelectedGameObjects(go, selectedGameObjects);

        ImGui::PushStyleColor(ImGuiCol_Button, {0, 0, 0, 0});
        if (ImGui::Button("Rename"))
        {
            m_IsRenaming = true;
            m_RenamingNeedsFocus = true;
            m_RenameTarget = go;
            ImGui::CloseCurrentPopup();
        }
        if (ImGui::Button("Delete"))
        {
            s_DestroyRequest = true;
            ImGui::CloseCurrentPopup();
        }
        ImGui::PopStyleColor();
        ImGui::EndPopup();
    }
}

void DrawInBetweenDropTarget()
{
    ImGui::InvisibleButton("In-between drop", {ImGui::GetContentRegionMax().x, 3});
}

void SceneHierarchyWindow::DrawGameObjectsHierarchy(std::vector<std::shared_ptr<GameObject>> &gameObjects, std::unordered_set<std::shared_ptr<GameObject>> &selectedGameObjects,
                                                    bool selectAll, bool selectRange, int startIndex, int endIndex)
{
    for (int i = startIndex; i < endIndex; ++i)
    {
        auto &go = gameObjects[i];
        auto &children = go->Children;

        bool isRenaming = DrawRenameInput(go);

        auto flags = GetFlags(selectedGameObjects.contains(go), !children.empty());
        auto label = GetLabel(go, isRenaming);

        bool opened = ImGui::TreeNodeEx(label.c_str(), flags);
        s_LastItemY = ImGui::GetCursorPos().y;

        DrawContextMenu(go, selectedGameObjects);
        HandleSelect(go, selectedGameObjects, selectAll, selectRange);
        HandleDrag(go, selectedGameObjects);
        HandleDrop(go);

        if (opened)
        {
            if (!children.empty())
            {
                DrawGameObjectsHierarchy(children, selectedGameObjects, selectAll, selectRange);
            }

            ImGui::TreePop();
        }

        DrawInBetweenDropTarget();
        HandleDrop(go->GetParent(), i + 1);
    }
}

void SceneHierarchyWindow::DrawGameObjectsHierarchy(std::vector<std::shared_ptr<GameObject>> &gameObjects, std::unordered_set<std::shared_ptr<GameObject>> &selectedGameObjects,
                                                    bool selectAll, bool selectRange)
{
    bool traverseAll = selectAll || selectRange;

    if (!gameObjects.empty())
    {
        DrawInBetweenDropTarget();
        HandleDrop(gameObjects[0]->GetParent(), 0);
    }

    if (traverseAll)
    {
        DrawGameObjectsHierarchy(gameObjects, selectedGameObjects, selectAll, selectRange, 0, gameObjects.size());
    }
    else
    {
        ImGuiListClipper clipper;
        clipper.Begin(gameObjects.size());

        while (clipper.Step())
        {
            DrawGameObjectsHierarchy(gameObjects, selectedGameObjects, selectAll, selectRange, clipper.DisplayStart, clipper.DisplayEnd);
        }
    }
}

bool ShouldSelectAll(std::unordered_set<std::shared_ptr<GameObject>> &selectedGameObjects)
{
    if (ImGui::GetIO().KeyCtrl && ImGui::IsKeyDown(ImGui::GetKeyIndex(ImGuiKey_A)))
    {
        selectedGameObjects.clear();
        return true;
    }

    return false;
}

bool ShouldSelectRange(std::unordered_set<std::shared_ptr<GameObject>> &selectedGameObjects)
{
    if (ImGui::GetIO().KeyShift && ImGui::IsMouseReleased(ImGuiMouseButton_Left) && !selectedGameObjects.empty())
    {
        s_SelectingRangeTarget = *selectedGameObjects.begin();
        return true;
    }

    return false;
}

void ProcessReparentRequest()
{
    if (s_ReparentRequest)
    {
        for (auto &draggedGo : s_SelectedGameObjectsCopy)
        {
            if (!draggedGo.expired())
            {
                draggedGo.lock()->SetParent(s_NewParent, s_ReparentIndex);
            }
            draggedGo.reset();
        }
    }
}

void ProcessDestroyRequest()
{
    if (s_DestroyRequest)
    {
        for (auto &destroyedGo : s_SelectedGameObjectsCopy)
        {
            if (!destroyedGo.expired())
            {
                destroyedGo.lock()->Destroy();
            }
            destroyedGo.reset();
        }
    }
}

void SceneHierarchyWindow::Reset()
{
    s_IsSelectingRange = false;
    s_SelectingRangeTarget = nullptr;
    if (!ImGui::GetDragDropPayload())
    {
        s_SelectedGameObjectsCopy.clear();
    }

    s_ReparentRequest = false;
    s_NewParent = nullptr;
    s_DestroyRequest = false;

    if (!m_IsRenaming)
    {
        m_RenamingNeedsFocus = false;
        m_RenameTarget = nullptr;
    }
}

void SceneHierarchyWindow::DrawInternal()
{
    auto &selectedGameObjects = Hierarchy::GetSelectedGameObjects();
    bool selectAll = ShouldSelectAll(selectedGameObjects);
    bool selectRange = ShouldSelectRange(selectedGameObjects);

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0.0f, 0.0f });
    if (ImGui::BeginChild("Hierarchy", {0, 0}, ImGuiChildFlags_Border))
    {
        DrawGameObjectsHierarchy(Scene::Current->GetRootGameObjects(), selectedGameObjects, selectAll, selectRange);
        HandleFreeDrop();
    }
    ImGui::EndChild();
    ImGui::PopStyleVar();

    ProcessReparentRequest();
    ProcessDestroyRequest();
    Reset();
}