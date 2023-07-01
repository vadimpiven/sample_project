# SPDX-License-Identifier: Apache-2.0 OR MIT

add_executable(filesystem_unit_tests)
add_test(NAME filesystem_unit_tests COMMAND filesystem_unit_tests)
target_link_libraries(filesystem_unit_tests
PRIVATE
    GTest::gmock_main
PRIVATE
    core
    filesystem
)
target_sources(filesystem_unit_tests
PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/directory_watcher/directory_watcher_factory_tests.cpp"
)
