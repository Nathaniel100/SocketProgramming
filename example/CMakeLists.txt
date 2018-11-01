

add_executable(SimpleClient simple_client.cpp)
target_link_libraries(SimpleClient libsocket)

add_executable(SimpleServer simple_server.cpp)
target_link_libraries(SimpleServer libsocket)

add_executable(SelectExample select_example.cpp)

add_executable(SelectClient select_client.cpp)
target_link_libraries(SelectClient libsocket)

add_executable(SelectServer select_server.cpp)
target_link_libraries(SelectServer libsocket)
