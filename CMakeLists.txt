cmake_minimum_required(VERSION 3.8)
project(SocketProgramming)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp)
add_executable(SocketProgramming ${SOURCE_FILES})

add_subdirectory(src)