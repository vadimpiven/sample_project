# SPDX-License-Identifier: Apache-2.0 OR MIT

add_library(filesystem STATIC)
generate_export_header(filesystem)
target_include_directories(core
PUBLIC
    "${CMAKE_CURRENT_LIST_DIR}/include"
    "${CMAKE_CURRENT_BINARY_DIR}"
)
target_link_libraries(filesystem
PRIVATE
    core
)
target_sources(filesystem
PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/sources/directory_watcher/directory_watcher_factory_impl.cpp"
)
if(APPLE)
    target_link_libraries(filesystem
    PUBLIC
        "-framework CoreServices"
    )
endif()

add_subdirectory(tests)
