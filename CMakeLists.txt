cmake_minimum_required(VERSION 3.16)
project(echo CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_compile_options(-stdlib=libc++)
    add_link_options(-stdlib=libc++)
endif()

add_library(echo_core STATIC
    src/echo_server.cpp
    src/file_descriptor.cpp
    src/tcp_listener.cpp
    src/tcp_connection.cpp
)
target_include_directories(echo_core PUBLIC src)
target_compile_features(echo_core PUBLIC cxx_std_20)
target_compile_options(echo_core PRIVATE -Wall -Wextra -Wpedantic)

add_executable(echo_server src/main.cpp)
target_link_libraries(echo_server PRIVATE echo_core)

option(ECHO_BUILD_TESTS "Build tests" ON)

if(ECHO_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
