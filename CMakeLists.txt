cmake_minimum_required(VERSION 3.20.3)

set(CMAKE_CONFIGURATION_TYPES RelWithDebInfo)
set(CMAKE_BUILD_TYPE RelWithDebInfo)
project(some_project
    VERSION 0.1.0
    DESCRIPTION "Some project"
)
set(CMAKE_CXX_STANDARD 20)

if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif()

include(FetchContent)
include(CTest)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/")
include(ExportHeader)
include(PlatformDefinition)
include(StrictCompilerChecks)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/3rdparty")
include(GTest)

add_subdirectory(libraries)
