if (${RENDER_ENGINE_ANDROID_PLATFORM})

    add_library(RenderEngineLauncher STATIC main.cpp)

    target_link_libraries(RenderEngineLauncher RenderEngine)
    target_include_directories(RenderEngineLauncher PUBLIC ${PROJECT_SOURCE_DIR}/engine_framework)

endif ()