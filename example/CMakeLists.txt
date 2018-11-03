

add_executable(SimpleClient simple_client.cpp)
target_link_libraries(SimpleClient libsocket)

add_executable(SimpleServer simple_server.cpp)
target_link_libraries(SimpleServer libsocket)

add_executable(SelectExample select_example.cpp)

add_executable(SelectClient select_client.cpp)
target_link_libraries(SelectClient libsocket)

add_executable(SelectServer select_server.cpp)
target_link_libraries(SelectServer libsocket)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LibEvent REQUIRED libevent)

message(STATUS "LibEvent: ${LibEvent_INCLUDE_DIRS} ${LibEvent_LIBRARIES}")
include_directories(${LibEvent_INCLUDE_DIRS})
add_executable(EventClient event_client.cpp)
target_link_libraries(EventClient ${LibEvent_LIBRARIES} libsocket)