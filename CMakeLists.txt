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

# 2. find zlib (required for Git object compression)
find_package(ZLIB REQUIRED)
# 3. find openSSL
find_package(OpenSSL REQUIRED)

# Define source library (Core Git logic)
add_library(git_core
    src/object.cpp
    src/refs.cpp
    src/repository.cpp
    src/index.cpp
)

# Link zlib to our core library
target_link_libraries(git_core PRIVATE ZLIB::ZLIB OpenSSL::Crypto)
target_include_directories(git_core PUBLIC include)

# Define main executable
add_executable(my_git src/main.cpp)
target_link_libraries(my_git PRIVATE git_core)

# Define unit tests
add_executable(git_tests
    tests/test_main.cpp
    tests/test_object.cpp
)
target_link_libraries(git_tests PRIVATE git_core gtest_main)

include(GoogleTest)
gtest_discover_tests(git_tests)