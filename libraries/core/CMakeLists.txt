add_library(core STATIC)
generate_export_header(core)
target_include_directories(core
PUBLIC
    "${CMAKE_CURRENT_LIST_DIR}/include"
    "${CMAKE_CURRENT_BINARY_DIR}"
)
target_sources(core
PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/sources/logging/logger_controller_impl.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/sources/logging/logger_impl.cpp"
)

add_executable(core_tests)
gtest_discover_tests(core_tests)
target_link_libraries(core_tests
PRIVATE
    GTest::gmock_main
PRIVATE
    core
)
target_sources(core_tests
PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/tests/logging/logger_controller_tests.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/tests/logging/logger_tests.cpp"
)
