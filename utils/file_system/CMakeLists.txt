add_library(FileSystem STATIC
    include/file_system.h
    include/file_watcher.h
    include/special_folder.h
    cpp/file_system.cpp
    cpp/file_watcher.cpp
	cpp/file_system_implementations/file_system_base.h
	cpp/file_system_implementations/file_system_base.cpp
	cpp/file_system_implementations/file_system_windows.cpp
	cpp/file_system_implementations/file_system_windows.h
	cpp/file_system_implementations/file_system_apple.cpp
	cpp/file_system_implementations/file_system_apple.h
	cpp/file_system_implementations/file_system_android.cpp
	cpp/file_system_implementations/file_system_android.h)

target_include_directories(FileSystem PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(FileSystem AppleFoundation)