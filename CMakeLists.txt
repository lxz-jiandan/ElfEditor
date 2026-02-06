cmake_minimum_required(VERSION 3.18)
project(ElfEditor)

set(CMAKE_CXX_STANDARD 14)

add_executable(ElfEditor
    main.cpp
    zElf.cpp
    zLog.cpp
)
