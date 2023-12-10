add_library(
	Editor
	editor_window.h
	editor_window.cpp
	hierarchy_widget/hierarchy_tree_widget.h
	hierarchy_widget/hierarchy_tree_widget.cpp
	hierarchy_widget/hierarchy_tree_widget_item.h
	hierarchy_widget/hierarchy_tree_widget_item.cpp
    hierarchy_widget/hierarchy_widget.h
    hierarchy_widget/hierarchy_widget.cpp
)

target_include_directories(Editor PUBLIC .)
target_link_libraries(Editor Core)
