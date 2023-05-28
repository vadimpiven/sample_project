# SPDX-License-Identifier: MIT

add_library(core STATIC)
generate_export_header(core)
target_include_directories(core
PUBLIC
    "${CMAKE_CURRENT_LIST_DIR}/include"
    "${CMAKE_CURRENT_BINARY_DIR}"
)
target_sources(core
PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/sources/logging/logger_impl.cpp"
)

add_subdirectory(tests)
