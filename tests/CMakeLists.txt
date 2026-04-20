project(plugini_tests LANGUAGES CXX)

add_library(testMain OBJECT testMain.cpp)
target_link_libraries(testMain PUBLIC Catch2::Catch2)

# Stable directory for test-only plugin binaries. The test_plugin target drops
# its shared library here and PluginLoadingTests scans it at runtime.
set(PLUGINI_TESTS_PLUGIN_DIR "${CMAKE_BINARY_DIR}/tests/plugins")
add_subdirectory(test_plugin)

list(APPEND CMAKE_MODULE_PATH ${Catch2_SOURCE_DIR}/extras)
include(CTest)
include(Catch)

if(BUILD_TESTING)
    file(GLOB_RECURSE TEST_SOURCES *Tests.cpp)
    foreach(testsourcefile ${TEST_SOURCES})
        get_filename_component(target ${testsourcefile} NAME_WE)
        add_executable(${target} ${testsourcefile})
        set_target_properties(${target} PROPERTIES CXX_EXTENSIONS OFF)
        target_compile_features(${target} PUBLIC cxx_std_23)
        target_link_libraries(${target} PRIVATE testMain plugini::plugini)
        target_compile_definitions(${target} PRIVATE
            PLUGINI_TESTS_PLUGIN_DIR="${PLUGINI_TESTS_PLUGIN_DIR}"
        )
        # Ensure the test plugin is built before any test executable runs.
        add_dependencies(${target} test_plugin)
        catch_discover_tests(
            ${target}
            TEST_PREFIX ${target}.
            REPORTER XML
            OUTPUT_DIR .
            OUTPUT_PREFIX ${target}
            OUTPUT_SUFFIX .xml
        )
    endforeach()
endif()
