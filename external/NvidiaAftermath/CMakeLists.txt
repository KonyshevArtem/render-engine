add_library(NvidiaAftermath STATIC dummy.cpp)

target_include_directories(NvidiaAftermath INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(NvidiaAftermath INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/lib/x64/GFSDK_Aftermath_Lib.x64.lib)

add_custom_command(TARGET NvidiaAftermath POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/x64/GFSDK_Aftermath_Lib.x64.dll"
        "$<TARGET_FILE_DIR:RenderEngineLauncher>")