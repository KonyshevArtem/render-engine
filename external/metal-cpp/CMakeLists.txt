add_library(metal-cpp INTERFACE)

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    target_include_directories(metal-cpp INTERFACE .)
    target_link_libraries(metal-cpp INTERFACE "-framework Foundation -framework QuartzCore -framework Metal")
endif ()