add_library(glfw INTERFACE)

target_link_libraries(glfw INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/windows/lib-vc2022/glfw3.lib)
target_include_directories(glfw INTERFACE include)