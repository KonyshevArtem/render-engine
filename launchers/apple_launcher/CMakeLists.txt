if (${RENDER_ENGINE_APPLE_PLATFORM})

    add_library(RenderEngineLauncher STATIC imgui_wrapper_apple.mm imgui_wrapper_apple.h)
    target_link_libraries(RenderEngineLauncher RenderEngine)

    if (${ENABLE_IMGUI})
        target_link_libraries(RenderEngineLauncher ImGuiMetal)
        target_compile_definitions(RenderEngineLauncher PRIVATE ENABLE_IMGUI)
    endif ()

endif ()