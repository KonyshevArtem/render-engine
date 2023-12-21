add_library(
        Utils
        utils.cpp
        utils.h
        texture_binary_reader.cpp
        texture_binary_reader.h
)

target_include_directories(Utils PUBLIC .)