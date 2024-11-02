add_library(DXC INTERFACE)

if (${RENDER_ENGINE_WINDOWS_PLATFORM})

    target_link_libraries(DXC INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/lib/windows/arch_x64/dxcompiler.lib)

elseif (${RENDER_ENGINE_APPLE_PLATFORM})

    target_link_libraries(DXC INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/lib/macos/arch_${CMAKE_HOST_SYSTEM_PROCESSOR}/libdxcompiler.dylib)
    target_include_directories(DXC INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include/stubs)
    target_compile_definitions(DXC INTERFACE __EMULATE_UUID INTERFACE COM_NO_WINDOWS_H)

endif()

target_include_directories(DXC INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include/dxc)