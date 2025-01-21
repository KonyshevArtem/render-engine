if (${RENDER_ENGINE_WINDOWS_PLATFORM})

    add_executable(RenderEngineLauncher main.cpp)
    set_target_properties(RenderEngineLauncher PROPERTIES WIN32_EXECUTABLE TRUE)

    target_link_libraries(RenderEngineLauncher RenderEngine)
    target_include_directories(RenderEngineLauncher PUBLIC ${PROJECT_SOURCE_DIR}/engine_framework)

    # copy resources to build directory
    add_custom_target(PreBuildWindows ALL
            COMMAND ${CMAKE_COMMAND} -E rm -rf $<TARGET_FILE_DIR:RenderEngineLauncher>/core_resources
            COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:RenderEngineLauncher>/core_resources
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/build_resources/windows/core_resources $<TARGET_FILE_DIR:RenderEngineLauncher>/core_resources
            COMMAND ${CMAKE_COMMAND} -E echo \"[Pre Build] Resources copied\")
    add_dependencies(RenderEngineLauncher PreBuildWindows)

endif ()