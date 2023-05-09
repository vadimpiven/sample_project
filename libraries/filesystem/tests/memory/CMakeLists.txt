add_executable(filesystem_memory_tests)
add_asan_test(NAME filesystem_memory_tests COMMAND filesystem_memory_tests)
target_link_libraries(filesystem_memory_tests
PRIVATE
    core
    filesystem
)
target_sources(filesystem_memory_tests
PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/directory_watcher/directory_watcher_factory_tests.cpp"
)
