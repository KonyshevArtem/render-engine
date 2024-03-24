list(
        APPEND
        IMGUI_SOURCES
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
        imgui/backends/imgui_impl_opengl3.h
        imgui/backends/imgui_impl_opengl3.cpp
        imgui/misc/cpp/imgui_stdlib.h
        imgui/misc/cpp/imgui_stdlib.cpp
)

if (${RENDER_ENGINE_WINDOWS_PLATFORM})

    set(IMGUI_LINK_LIBS glfw)
    list(APPEND IMGUI_SOURCES imgui/backends/imgui_impl_glfw.h imgui/backends/imgui_impl_glfw.cpp)
    set(IMGUI_COMPILE_DEFINITIONS "")

elseif (${RENDER_ENGINE_APPLE_PLATFORM})

    set(IMGUI_LINK_LIBS metal-cpp)
    list(
            APPEND
            IMGUI_SOURCES
            imgui/backends/imgui_impl_osx.h
            imgui/backends/imgui_impl_osx.mm
            imgui/backends/imgui_impl_metal.h
            imgui/backends/imgui_impl_metal.mm
    )
    set(IMGUI_COMPILE_DEFINITIONS IMGUI_IMPL_METAL_CPP IMGUI_IMPL_METAL_CPP_EXTENSIONS)

endif ()

add_library(ImGui STATIC ${IMGUI_SOURCES})

target_compile_definitions(ImGui PUBLIC ${IMGUI_COMPILE_DEFINITIONS})
target_link_libraries(ImGui ${IMGUI_LINK_LIBS})
target_include_directories(ImGui PUBLIC imgui imgui/backends imgui/misc/cpp)