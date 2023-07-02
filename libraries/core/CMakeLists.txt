# SPDX-License-Identifier: Apache-2.0 OR MIT

add_library(core INTERFACE)
target_include_directories(core
INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/include"
)
