add_library(metal-cpp INTERFACE)
add_library(AppleFoundation INTERFACE)

if (${RENDER_ENGINE_APPLE_PLATFORM})
    target_include_directories(metal-cpp INTERFACE .)
    target_link_libraries(metal-cpp INTERFACE "-framework Foundation -framework QuartzCore -framework Metal -framework MetalKit")

    target_include_directories(AppleFoundation INTERFACE .)
    target_link_libraries(AppleFoundation INTERFACE "-framework Foundation")
endif ()