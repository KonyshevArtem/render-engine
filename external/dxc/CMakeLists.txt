add_library(DXC INTERFACE)

target_link_libraries(DXC INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/lib/macos/libdxcompiler.dylib)
target_include_directories(DXC INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_definitions(DXC INTERFACE __EMULATE_UUID INTERFACE COM_NO_WINDOWS_H)