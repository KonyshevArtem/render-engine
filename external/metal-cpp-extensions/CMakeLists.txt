add_library(metal-cpp-extensions INTERFACE)

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    target_include_directories(metal-cpp-extensions INTERFACE .)
    target_link_libraries(metal-cpp-extensions INTERFACE "-framework AppKit -framework MetalKit")
endif ()