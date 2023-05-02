cmake_minimum_required(VERSION 3.20.3)

set(CMAKE_CONFIGURATION_TYPES RelWithDebInfo)
set(CMAKE_BUILD_TYPE RelWithDebInfo)
project(some_project
    VERSION 0.1.0
    DESCRIPTION "Some project"
)
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${CMAKE_SOURCE_DIR}/cmake/")
include(ExportHeader)
include(PlatformDefinition)
include(GTest)

add_subdirectory(libraries)
