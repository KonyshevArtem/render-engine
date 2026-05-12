if (${RENDER_ENGINE_WINDOWS_PLATFORM})

    add_executable(RenderEngineLauncher main.cpp)
    set_target_properties(RenderEngineLauncher PROPERTIES WIN32_EXECUTABLE TRUE)

    target_link_libraries(RenderEngineLauncher RenderEngine)
    target_include_directories(RenderEngineLauncher PUBLIC ${PROJECT_SOURCE_DIR}/engine_framework)         

    if (${RENDER_ENGINE_EDITOR})
        add_custom_command(TARGET RenderEngineLauncher POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E rm -rf $<TARGET_FILE_DIR:RenderEngineLauncher>/core_resources
                COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:RenderEngineLauncher>/editor_resources
                COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_SOURCE_DIR}/core_resources $<TARGET_FILE_DIR:RenderEngineLauncher>/editor_resources/core_resources
                COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_SOURCE_DIR}/build_resources/windows/core_resources $<TARGET_FILE_DIR:RenderEngineLauncher>/core_resources
                COMMAND ${CMAKE_COMMAND} -E echo \"[Pre Build] Editor resources setup\")   
    else()
        add_custom_command(TARGET RenderEngineLauncher POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E rm -rf $<TARGET_FILE_DIR:RenderEngineLauncher>/core_resources
                COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:RenderEngineLauncher>/core_resources
                COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/build_resources/windows/core_resources $<TARGET_FILE_DIR:RenderEngineLauncher>/core_resources
                COMMAND ${CMAKE_COMMAND} -E echo \"[Pre Build] Resources copied\")
    endif()

endif ()