cmake_minimum_required(VERSION 3.22) 
project(Gng2D)
include(FetchContent)
set(CMAKE_EXPORT_COMPILE_COMMANDS True)

### 3RD PARTY LIBRARIES #######################################################
cmake_policy(SET CMP0135 NEW)
# EnTT
FetchContent_Declare(
    EnTT
    GIT_REPOSITORY  https://github.com/skypjack/entt.git
    GIT_TAG         v3.12.2)

# GoogleTest
include(GoogleTest)
FetchContent_Declare(
  googletest
  URL               https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip)

# Lua
FetchContent_Declare(
    lua 
    URL https://www.lua.org/ftp/lua-5.4.6.tar.gz
    URL_HASH SHA256=7d5ea1b9cb6aa0b59ca3dde1c6adcb57ef83a1ba8e5432c0ecd06bf439b3ad88)

# MAKE AVAILABLE
FetchContent_MakeAvailable(EnTT googletest lua)

# Build lua
file(GLOB lua_src 
    "${lua_SOURCE_DIR}/src/*.c"
    "${lua_SOURCE_DIR}/src/*.cpp")
add_library(lua ${lua_src})
target_include_directories(lua INTERFACE "${lua_SOURCE_DIR}/src")

### Gng2D #####################################################################
add_library(Gng2D)
target_link_libraries(Gng2D
    SDL2
    SDL2_image
    EnTT::EnTT
    lua)
set_property(TARGET Gng2D PROPERTY CXX_STANDARD 20)
set_property(TARGET Gng2D PROPERTY ENABLE_EXPORTS TRUE)
add_compile_options(-Wall -Wextra -Wpedantic)
add_executable(Gng2D_EXEC)
target_link_libraries(Gng2D_EXEC
    Gng2D)

### SANDBOX ###################################################################
if(NOT DEFINED GAME_NAME)
    set_property(TARGET Gng2D_EXEC PROPERTY OUTPUT_NAME "sandbox")
    set_property(TARGET Gng2D_EXEC PROPERTY CXX_STANDARD 20)
    set_property(TARGET Gng2D_EXEC PROPERTY RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/sandbox")
    add_custom_command(TARGET Gng2D_EXEC POST_BUILD
        COMMAND Gng2D_EXEC
        COMMENT "RUNNING GNG2D-SANDBOX")
endif()

### TESTING ###################################################################
enable_testing()
add_executable(Gng2D_TESTS EXCLUDE_FROM_ALL)
target_link_libraries(Gng2D_TESTS
    Gng2D
    GTest::gtest_main
    GTest::gmock_main)
set(TEST_FILES_DIR "${CMAKE_BINARY_DIR}/testdir")
target_compile_definitions(Gng2D_TESTS PRIVATE   
    "TEST_FILES_DIR=\"${TEST_FILES_DIR}\"")      
gtest_discover_tests(Gng2D_TESTS)
set_property(TARGET Gng2D_TESTS PROPERTY CXX_STANDARD 20)
add_custom_target(run_tests
    COMMAND ${CMAKE_COMMAND} -E make_directory ${TEST_FILES_DIR}
    COMMAND Gng2D_TESTS)

add_subdirectory(source)

