add_library(
        DebugUtil
        debug.h
        debug.cpp
)

add_library(
        Hash
        hash.h
        hash.cpp
)

target_include_directories(DebugUtil PUBLIC .)
target_include_directories(Hash PUBLIC .)