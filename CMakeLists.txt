﻿# SPDX-License-Identifier: Apache-2.0 OR MIT

cmake_minimum_required(VERSION 3.21.0)

project(sample-project
    VERSION 0.1.0
    DESCRIPTION "Sample Project"
)
set(CMAKE_CXX_STANDARD 20)

if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif()

include(CTest)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/")
include(ExportHeader)
include(PlatformDefinition)
include(StrictCompilerChecks)
include(CharAsUtf8)

find_package(GTest CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

add_subdirectory(libraries)
