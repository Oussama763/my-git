cmake_minimum_required(VERSION 3.14)
project(my_git CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable testing framework
enable_testing()

# Include FetchContent for automatic dependency management
include(FetchContent)

# 1. Fetch GoogleTest
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
# Prevent gtest from overriding our compiler/runtime settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# 2. Fetch zlib (required for Git object compression)
FetchContent_Declare(
    zlib
    URL https://github.com/madler/zlib/archive/refs/tags/v1.3.1.zip
)
FetchContent_MakeAvailable(zlib)

# Define source library (Core Git logic)
add_library(git_core
    src/object.cpp
    src/refs.cpp
    src/repository.cpp
    src/index.cpp
)

# Link zlib to our core library
target_link_libraries(git_core PRIVATE zlib)
target_include_directories(git_core PUBLIC include)

# Define main executable
add_executable(my_git src/main.cpp)
target_link_libraries(my_git PRIVATE git_core)

# Define unit tests
add_executable(git_tests
    tests/test_main.cpp
)
target_link_libraries(git_tests PRIVATE git_core gtest_main)

include(GoogleTest)
gtest_discover_tests(git_tests)