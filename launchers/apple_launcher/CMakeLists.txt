if (${RENDER_ENGINE_APPLE_PLATFORM})

    add_library(RenderEngineLauncher main.cpp)
    target_link_libraries(RenderEngineLauncher RenderEngine)

endif ()