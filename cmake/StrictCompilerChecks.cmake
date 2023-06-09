﻿# SPDX-License-Identifier: Apache-2.0 OR MIT

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

macro(enable_strict_compiler_checks)
    if(MSVC)
        add_compile_definitions(__STDC_WANT_SECURE_LIB__)
        add_compile_options(/sdl /W4 /WX)
        add_link_options(/WX)
    else()
        add_compile_options(-Wall -Wextra -Werror -pedantic -pedantic-errors)
    endif()
endmacro()

option(ASAN "Address sanitizing" OFF)
if(ASAN)
    message("-- Address sanitizing: ON")
    if(MSVC)
        add_compile_options(/fsanitize=address)
        add_link_options(/IGNORE:4300)
    else()
        add_compile_options(-fsanitize=address)
        add_link_options(-fsanitize=address)
    endif()
else()
    message("-- Address sanitizing: OFF (use -D ASAN=ON to enable)")
endif()

function(add_asan_test)
    set(oneValueArgs NAME COMMAND)
    cmake_parse_arguments(ARG "" "${oneValueArgs}" "" "${ARGN}")

    if(ASAN)
        add_test(NAME "${ARG_NAME}" COMMAND "${ARG_COMMAND}")
        if(NOT MSVC AND NOT CMAKE_CXX_COMPILER_ID MATCHES "AppleClang")
            set_property(TEST "${ARG_NAME}" PROPERTY ENVIRONMENT ASAN_OPTIONS=detect_leaks=1)
        endif()
    endif()
endfunction()
