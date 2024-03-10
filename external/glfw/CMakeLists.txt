add_library(glfw INTERFACE)

include_directories(include)
target_link_libraries(glfw INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/lib-mingw-w64/libglfw3.a)

target_include_directories(glfw INTERFACE include)