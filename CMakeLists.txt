cmake_minimum_required(VERSION 3.22) 
project(Gng2D)
include(FetchContent)

### 3RD PARTY LIBRARIES #######################################################
# EnTT
FetchContent_Declare(
    EnTT
    GIT_REPOSITORY  https://github.com/skypjack/entt.git
    GIT_TAG         v3.12.2)
FetchContent_MakeAvailable(Entt)

# GoogleTest
include(GoogleTest)
FetchContent_Declare(
  googletest
  URL               https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip)
FetchContent_MakeAvailable(googletest)

### Gng2D #####################################################################
add_executable(Gng2D)
target_link_libraries(Gng2D
    SDL2
    SDL2_image
    EnTT::EnTT)
set_property(TARGET Gng2D PROPERTY CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS True)
add_compile_options(-Wall -Wextra -Wpedantic)

### SANDBOX ###################################################################
if(NOT DEFINED GAME_NAME)
    set_property(TARGET Gng2D PROPERTY OUTPUT_NAME "sandbox")
    set_property(TARGET Gng2D PROPERTY RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/sandbox")
    add_custom_command(TARGET Gng2D POST_BUILD
        COMMAND Gng2D
        COMMENT "RUNNING GNG2D-SANDBOX")
endif()

### TESTING ###################################################################
#enable_testing()
#add_executable(Gng2D_TESTS EXCLUDE_FROM_ALL)
#target_link_libraries(Gng2D_TESTS
#    Gng2D
#    GTest::gtest_main
#    GTest::gmock_main)
#gtest_discover_tests(Gng2D_TESTS)
#set_property(TARGET Gng2D_TESTS PROPERTY CXX_STANDARD 20)
#add_custom_target(Gng2D_run_test
#    COMMAND Gng2D_TESTS)

add_subdirectory(source)

