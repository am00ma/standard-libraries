cmake_minimum_required(VERSION 3.15)

# =========================================================================
#                       Project
# =========================================================================
project(
    "stz-cpp"
    VERSION 0.0.1
    DESCRIPTION "Standard library for C++"
    LANGUAGES CXX)

# =========================================================================
#                      Options
# =========================================================================
option(STZ_BUILD_TESTS "Build tests" ON)

# =========================================================================
#                      stz
# =========================================================================
set(name stz-cpp)

add_library(${name} SHARED)

set_target_properties(${name} PROPERTIES LANGUAGES       CXX)
set_target_properties(${name} PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(${name} PROPERTIES CXX_STANDARD    23)

target_sources(${name}
    PRIVATE

    PUBLIC
    FILE_SET HEADERS
    BASE_DIRS
        include
    FILES
        include/stz-cpp/stz-cpp.hpp
        include/stz-cpp/test.hpp

)

target_include_directories(${name}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(${name}
    PUBLIC
        m
        # md4c
        # sqlite3
        # SDL2
        # SDL2_ttf
        # SDL2_image
)

target_compile_options(${name}
    PRIVATE
        -O0
        -ggdb
        -Wall
        -Wextra
        -fdiagnostics-color=always)

# =========================================================================
#                      tests
# =========================================================================
if(STZ_BUILD_TESTS)
    add_subdirectory(tests)
endif()
