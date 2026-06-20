cmake_minimum_required(VERSION 3.16)
project(echo CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(echo_server
    main.cpp
    echo_server.cpp
    file_descriptor.cpp
    tcp_listener.cpp
    tcp_connection.cpp
)
