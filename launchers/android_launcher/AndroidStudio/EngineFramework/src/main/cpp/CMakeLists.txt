cmake_minimum_required(VERSION 3.22.1)

project("engineframework")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(FREETYPE_LIB freetyped)
else()
    set(FREETYPE_LIB freetype)
endif()

set(
        DEPENDENCIES
        Core
        DebugUtil
        GameWindow
        GraphicsBackend
        ImGuiWrapper
        Math
        RenderEngine
        RenderEngineLauncher
        Arguments
        Hash
        ${FREETYPE_LIB}
        harfbuzz
        trex
        StringEncodingUtil
        StringSplit
)

foreach (LIB ${DEPENDENCIES})

    add_library(${LIB} STATIC IMPORTED)
    set_target_properties(${LIB} PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/libs/${CMAKE_ANDROID_ARCH_ABI}/${CMAKE_BUILD_TYPE}/lib${LIB}.a)

endforeach ()

add_library(${CMAKE_PROJECT_NAME} SHARED engine_framework.cpp native_bridge.cpp)
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