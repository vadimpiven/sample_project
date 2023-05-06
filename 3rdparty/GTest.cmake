FetchContent_Declare(googletest
    GIT_REPOSITORY git@github.com:google/googletest.git
    GIT_TAG v1.13.0
    OVERRIDE_FIND_PACKAGE
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
enable_testing()
include(GoogleTest)
