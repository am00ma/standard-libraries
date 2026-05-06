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
        include/stz-cpp/types.hpp
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
        # -fsanitize=address
        -fdiagnostics-color=always)
# target_link_options(${name} PRIVATE -fsanitize=address)

# =========================================================================
#                      Installation
# =========================================================================
install(
    TARGETS ${name}
    EXPORT ${name}Targets
    FILE_SET HEADERS
)

include(GNUInstallDirs)

install(
    EXPORT ${name}Targets
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${name}
    NAMESPACE ${name}::
)

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/${name}ConfigVersion.cmake
    COMPATIBILITY ExactVersion
)

install(
    FILES
        cmake/${name}Config.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/${name}ConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${name}
)

configure_file(cmake/${name}.pc.in ${name}.pc @ONLY)

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/${name}.pc
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
)

# Otherwise system does not pick it up
install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/${name}.pc
    DESTINATION /usr/lib/pkgconfig
)


# =========================================================================
#                      tests
# =========================================================================
if(STZ_BUILD_TESTS)
    add_subdirectory(tests)
endif()
