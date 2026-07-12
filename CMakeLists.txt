cmake_minimum_required(VERSION 3.16)
project(echo CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native")

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_compile_options(-stdlib=libc++)
    add_link_options(-stdlib=libc++)
endif()

add_library(echo_common INTERFACE)
target_include_directories(echo_common INTERFACE ${CMAKE_SOURCE_DIR}/common)



add_library(echo_core STATIC
    src/echo_server.cpp
    src/file_descriptor.cpp
    src/tcp_listener.cpp
    src/tcp_connection.cpp
)
target_include_directories(echo_core PUBLIC src)
target_compile_options(echo_core PRIVATE -Wall -Wextra -Wpedantic)
target_link_libraries(echo_core PUBLIC echo_common)

add_executable(echo_server src/main.cpp)
target_link_libraries(echo_server PRIVATE echo_core)

add_executable(load_client tools/load_client.cpp)
target_link_libraries(load_client PRIVATE echo_common)
target_compile_options(load_client PRIVATE -Wall -Wextra)

add_executable(noise_floor tools/noise_floor.cpp)
target_link_libraries(noise_floor PRIVATE echo_common)

option(ECHO_BUILD_TESTS "Build tests" ON)
if(ECHO_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
