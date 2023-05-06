FetchContent_Declare(googletest
    # b796f7d44681514f58a683a3a71ff17c94edb0c1 is a commit hash for tag v1.13.0
    URL https://github.com/google/googletest/archive/b796f7d44681514f58a683a3a71ff17c94edb0c1.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
enable_testing()
include(GoogleTest)
