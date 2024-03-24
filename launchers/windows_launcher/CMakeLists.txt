if (${RENDER_ENGINE_WINDOWS_PLATFORM})

    add_executable(RenderEngineWindows main.cpp)

    target_link_libraries(RenderEngineWindows RenderEngine ImGuiGLFW ImGuiOpenGL)
    target_include_directories(RenderEngineWindows PUBLIC ${PROJECT_SOURCE_DIR}/engine_framework)

    # copy resources to build directory
    add_custom_target(PreBuildWindows ALL
            COMMAND ${CMAKE_COMMAND} -E rm -rf $<TARGET_FILE_DIR:RenderEngineWindows>/resources
            COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:RenderEngineWindows>/resources
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/resources $<TARGET_FILE_DIR:RenderEngineWindows>/resources
            COMMAND ${CMAKE_COMMAND} -E echo \"[Pre Build] Resources copied\")
    add_dependencies(RenderEngineWindows PreBuildWindows)

endif ()