cmake_minimum_required(VERSION 3.22.1)

project("engineframework")

set(
        DEPENDENCIES
        Core
        DebugUtil
        GameWindow
        GraphicsBackend
        ImGuiWrapper
        Math
        OpenFBX
        RenderEngine
        RenderEngineLauncher
        Arguments
        Hash
        freetype
        harfbuzz
        trex
)

foreach (LIB ${DEPENDENCIES})

    add_library(${LIB} STATIC IMPORTED)
    set_target_properties(${LIB} PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/libs/${CMAKE_ANDROID_ARCH_ABI}/${CMAKE_BUILD_TYPE}/lib${LIB}.a)

endforeach ()

add_library(${CMAKE_PROJECT_NAME} SHARED engine_framework.cpp)
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE include)
target_link_libraries(
        ${CMAKE_PROJECT_NAME}
        android
        log
        z
        ${DEPENDENCIES}
        GLESv2
        GLESv3
        EGL
)