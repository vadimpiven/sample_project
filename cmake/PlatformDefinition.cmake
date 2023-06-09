﻿# SPDX-License-Identifier: Apache-2.0 OR MIT

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(PLATFORM "linux")
    add_compile_definitions(P_LINUX)
endif()
if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(PLATFORM "macos")
    add_compile_definitions(P_MACOS)
endif()
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(PLATFORM "windows")
    add_compile_definitions(P_WINDOWS)
endif()
