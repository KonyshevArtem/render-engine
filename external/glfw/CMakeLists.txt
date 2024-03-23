add_library(glfw INTERFACE)

target_link_libraries(glfw INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/windows/lib-mingw-w64/libglfw3.a)
target_include_directories(glfw INTERFACE include)