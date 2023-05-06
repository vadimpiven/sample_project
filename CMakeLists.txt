cmake_minimum_required(VERSION 3.20.3)

set(CMAKE_CONFIGURATION_TYPES RelWithDebInfo)
set(CMAKE_BUILD_TYPE RelWithDebInfo)
project(some_project
    VERSION 0.1.0
    DESCRIPTION "Some project"
)
set(CMAKE_CXX_STANDARD 20)

include(CTest)
if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.24.0")
    cmake_policy(SET CMP0135 NEW) # Avoid warning about DOWNLOAD_EXTRACT_TIMESTAMP in CMake 3.24:
endif()
include(FetchContent)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/")
include(ExportHeader)
include(PlatformDefinition)
include(StrictCompilerChecks)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/3rdparty")
include(GTest)

add_subdirectory(libraries)
