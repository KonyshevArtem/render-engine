if (${RENDER_ENGINE_APPLE_PLATFORM})

    add_library(RenderEngineApple STATIC imgui_wrapper_apple.mm imgui_wrapper_apple.h)
    target_link_libraries(RenderEngineApple RenderEngine ImGuiOSX ImGuiMetal)

endif ()