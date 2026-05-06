cmake_minimum_required(VERSION 3.15)

# =========================================================================
#                       Project
# =========================================================================
project(
    "poc"
    VERSION 0.0.1
    DESCRIPTION "Proof of concept programs."
    LANGUAGES C CXX)

# =========================================================================
#                       Proof of concepts
# =========================================================================
file(GLOB srcs "poc_*.c")

foreach(path ${srcs})

    get_filename_component(name ${path} NAME_WLE)

    add_executable(${name} ${path})

    set_target_properties(${name} PROPERTIES LANGUAGES       C)
    set_target_properties(${name} PROPERTIES LINKER_LANGUAGE C)
    set_target_properties(${name} PROPERTIES C_STANDARD      23)

    target_compile_options(${name}
        PRIVATE
            -O0
            -ggdb
            -Wall
            -Wextra
            -fdiagnostics-color=always)

endforeach()
