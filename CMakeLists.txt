cmake_minimum_required(VERSION 3.22) 
project(Gng2D)
include(FetchContent)

# EnTT
FetchContent_Declare(
    EnTT
    GIT_REPOSITORY  https://github.com/skypjack/entt.git
    GIT_TAG         fef921132cae7588213d0f9bcd2fb9c8ffd8b7fc # 3.11.1
)
FetchContent_MakeAvailable(Entt)

# GoogleTest
include(GoogleTest)
FetchContent_Declare(
  googletest
  URL               https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
FetchContent_MakeAvailable(googletest)

# Gng2D
add_library(Gng2D)
target_link_libraries(Gng2D
    SDL2
    SDL2_image
    EnTT::EnTT)
set_property(TARGET Gng2D PROPERTY CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS True)
add_compile_options(-Wall -Wextra -Wpedantic)

# Testing
enable_testing()
add_executable(Gng2D_TESTS EXCLUDE_FROM_ALL)
target_link_libraries(Gng2D_TESTS
    Gng2D
    GTest::gtest_main
    GTest::gmock_main)
gtest_discover_tests(Gng2D_TESTS)
set_property(TARGET Gng2D_TESTS PROPERTY CXX_STANDARD 20)
add_custom_target(Gng2D_run_test
    COMMAND Gng2D_TESTS)

add_subdirectory(source)

