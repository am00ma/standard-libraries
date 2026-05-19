cmake_minimum_required(VERSION 3.15)

# =========================================================================
#                       Project
# =========================================================================
project(
    "mdcode"
    VERSION 0.0.1
    DESCRIPTION "Render code to/from markdown"
    LANGUAGES CXX)

add_subdirectory(../../repos/stz-cpp/ ./stz-cpp/)

# =========================================================================
#                       Binary: mdcode
# =========================================================================
set(name mdcode)

add_executable(${name} main.cpp)

set_target_properties(${name} PROPERTIES LANGUAGES       CXX)
set_target_properties(${name} PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(${name} PROPERTIES CXX_STANDARD    23)

target_link_libraries(${name} PRIVATE stz-cpp)

target_compile_options(${name}
    PRIVATE
        -O0
        -ggdb
        -Wall
        -Wextra
        -fdiagnostics-color=always)
