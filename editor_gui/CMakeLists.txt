add_library(
	Editor
	editor_window.h
	editor_window.cpp
    hierarchy_gui/hierarchy_gui.h
    hierarchy_gui/hierarchy_gui.cpp
)

target_include_directories(Editor PUBLIC .)
target_link_libraries(Editor Core Qt6::Widgets)
