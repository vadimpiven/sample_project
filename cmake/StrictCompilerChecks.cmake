﻿set(CMAKE_CXX_EXTENSIONS OFF)

macro(enable_strict_compiler_checks)
    if(MSVC)
        add_compile_definitions(__STDC_WANT_SECURE_LIB__)
        add_compile_options(/sdl /W4 /WX)
        add_link_options(/WX)
    else()
        add_compile_options(-Wall -Wextra -Werror -pedantic -pedantic-errors)
    endif()
endmacro()

function(ctest_sanitize_executable TARGET_NAME)
    if(MSVC)
        target_compile_options("${TARGET_NAME}" PRIVATE /fsanitize=address)
    else()
        target_compile_options("${TARGET_NAME}" PRIVATE -fsanitize=address)
        target_link_options("${TARGET_NAME}" PRIVATE -fsanitize=address)
    endif()

    add_test(NAME "${TARGET_NAME}_asan" COMMAND "${TARGET_NAME}")
    if(NOT MSVC AND NOT CMAKE_CXX_COMPILER_ID MATCHES "AppleClang")
        set_property(TEST "${TARGET_NAME}_asan" PROPERTY ENVIRONMENT ASAN_OPTIONS=detect_leaks=1)
    endif()
endfunction()
