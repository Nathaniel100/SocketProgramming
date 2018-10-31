cmake_minimum_required(VERSION 3.8)
project(SocketProgramming)

set(CMAKE_CXX_STANDARD 11)

include_directories(${PROJECT_SOURCE_DIR}/include)

set(SOCKET_HEADERS
        include/socket/log.h
        include/socket/result.h
        include/socket/socket.h)
set(SOCKET_SRCS src/socket.cpp)
add_library(libsocket ${SOCKET_HEADERS} ${SOCKET_SRCS})

add_subdirectory(example)
