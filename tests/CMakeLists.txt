include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.15.2
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(echo_tests
    test_file_descriptor.cpp
)
target_link_libraries(echo_tests PRIVATE echo_core GTest::gtest_main)
include(GoogleTest)
gtest_discover_tests(echo_tests)