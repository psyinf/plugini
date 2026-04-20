cmake_minimum_required(VERSION 3.22)
project(plugini VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(GNUInstallDirs)
include(cmake/detect_embedded.cmake)

option(PLUGINI_ENABLE_TESTING  "Enable tests"    ON)
option(PLUGINI_BUILD_EXAMPLES  "Build examples"  ${IS_STANDALONE_PROJECT})
option(PLUGINI_INSTALL         "Generate install rules" ${IS_STANDALONE_PROJECT})

include(cmake/PreventInSourceBuilds.cmake)
if(IS_STANDALONE_PROJECT)
    include(cmake/coverage.cmake)
endif()
include(cmake/setup_cpm.cmake)
include(cmake/deps_cpm.cmake)

add_subdirectory(libs)

if(PLUGINI_ENABLE_TESTING AND IS_STANDALONE_PROJECT)
    enable_testing()
    add_subdirectory(tests)
endif()
