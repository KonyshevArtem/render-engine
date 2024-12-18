add_library(
        ImGui
        imgui/imconfig.h
        imgui/imgui.h
        imgui/imgui.cpp
        imgui/imgui_demo.cpp
        imgui/imgui_draw.cpp
        imgui/imgui_internal.h
        imgui/imgui_tables.cpp
        imgui/imgui_widgets.cpp
        imgui/imstb_rectpack.h
        imgui/imstb_textedit.h
        imgui/imstb_truetype.h
        imgui/misc/cpp/imgui_stdlib.h
        imgui/misc/cpp/imgui_stdlib.cpp
)

target_include_directories(ImGui PUBLIC imgui imgui/misc/cpp)

if (${RENDER_ENGINE_WINDOWS_PLATFORM})

    add_library(
            ImGuiOpenGL
            STATIC
            imgui/backends/imgui_impl_opengl3.h
            imgui/backends/imgui_impl_opengl3.cpp
    )

    add_library(
            ImGuiGLFW
            STATIC
            imgui/backends/imgui_impl_glfw.h
            imgui/backends/imgui_impl_glfw.cpp
    )
    target_link_libraries(ImGuiGLFW ImGui glfw)
    target_link_libraries(ImGuiOpenGL ImGui)
    target_include_directories(ImGuiGLFW PUBLIC imgui/backends)
    target_include_directories(ImGuiOpenGL PUBLIC imgui/backends)

elseif (${RENDER_ENGINE_APPLE_PLATFORM})

    add_library(
            ImGuiMetal
            STATIC
            imgui/backends/imgui_impl_metal.h
            imgui/backends/imgui_impl_metal.mm
    )

    add_library(
            ImGuiOSX
            STATIC
            imgui/backends/imgui_impl_osx.h
            imgui/backends/imgui_impl_osx.mm
    )
    target_link_libraries(ImGuiOSX ImGui)
    target_link_libraries(ImGuiMetal ImGui)
    target_include_directories(ImGuiOSX PUBLIC imgui/backends)
    target_include_directories(ImGuiMetal PUBLIC imgui/backends)

endif ()