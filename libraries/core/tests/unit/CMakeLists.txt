# SPDX-License-Identifier: Apache-2.0 OR MIT

add_executable(core_unit_tests)
add_test(NAME core_unit_tests COMMAND core_unit_tests)
target_link_libraries(core_unit_tests
PRIVATE
    GTest::gmock_main
PRIVATE
    core
)
target_sources(core_unit_tests
PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/logging/logger_tests.cpp"
)
