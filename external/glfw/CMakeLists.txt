add_library(glfw INTERFACE)

include_directories(include)

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    target_link_libraries(glfw INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/windows/lib-mingw-w64/libglfw3.a)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    target_link_libraries(glfw INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/macos/lib-universal/libglfw3.a)
    target_link_libraries(glfw INTERFACE "-framework Cocoa -framework IOKit")
endif()

target_include_directories(glfw INTERFACE include)