add_executable(Sandbox)
set_property(TARGET Sandbox PROPERTY OUTPUT_NAME "sandbox")
set_property(TARGET Sandbox PROPERTY CXX_STANDARD 20)
set_property(TARGET Sandbox PROPERTY RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/sandbox")
add_custom_command(TARGET Sandbox POST_BUILD
    COMMAND Sandbox
    COMMENT "RUNNING GNG2D-SANDBOX")
target_link_libraries(Sandbox
    Gng2D)

add_subdirectory(source)

