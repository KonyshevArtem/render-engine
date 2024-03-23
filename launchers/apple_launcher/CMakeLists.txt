if (${RENDER_ENGINE_APPLE_PLATFORM})

    add_library(RenderEngineApple STATIC main.cpp)
    target_link_libraries(RenderEngineApple RenderEngine)

endif ()