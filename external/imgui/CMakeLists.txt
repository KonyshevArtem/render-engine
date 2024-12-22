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

    set(
            IMGUI_WRAPPER_SOURCES
            imgui_wrapper.h
            imgui_wrapper_common.h
            imgui_wrapper_windows.cpp
            imgui/backends/imgui_impl_opengl3.h
            imgui/backends/imgui_impl_opengl3.cpp
            imgui/backends/imgui_impl_glfw.h
            imgui/backends/imgui_impl_glfw.cpp
    )

    set(IMGUI_WRAPPER_LIBS ImGui glfw)

elseif (${RENDER_ENGINE_APPLE_PLATFORM})

    set(
            IMGUI_WRAPPER_SOURCES
            imgui_wrapper.h
            imgui_wrapper_common.h
            imgui_wrapper_apple.cpp
            imgui/backends/imgui_impl_metal.h
            imgui/backends/imgui_impl_metal.mm
            imgui/backends/imgui_impl_osx.h
            imgui/backends/imgui_impl_osx.mm
    )

    set(IMGUI_WRAPPER_LIBS ImGui metal-cpp)
    set(IMGUI_WRAPPER_DEFINES IMGUI_IMPL_METAL_CPP IMGUI_IMPL_METAL_CPP_EXTENSIONS)

endif ()

add_library(ImGuiWrapper STATIC ${IMGUI_WRAPPER_SOURCES})
target_link_libraries(ImGuiWrapper ${IMGUI_WRAPPER_LIBS})
target_compile_definitions(ImGuiWrapper PRIVATE ${IMGUI_WRAPPER_DEFINES})
target_include_directories(ImGuiWrapper PUBLIC ./ imgui/backends)

if (${ENABLE_IMGUI})
    target_compile_definitions(ImGuiWrapper PRIVATE ENABLE_IMGUI)
endif ()