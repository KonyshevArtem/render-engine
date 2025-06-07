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

add_library(
        Arguments
        arguments.h
        arguments.cpp
)

add_library(
        StringSplit
        string_split.h
        string_split.cpp
)

target_include_directories(DebugUtil PUBLIC .)
target_include_directories(Hash PUBLIC .)
target_include_directories(Arguments PUBLIC .)
target_include_directories(StringSplit PUBLIC .)