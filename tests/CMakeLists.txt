project(plugini_tests LANGUAGES CXX)

add_library(testMain OBJECT testMain.cpp)
target_link_libraries(testMain PUBLIC Catch2::Catch2)

# Stable directory for test-only plugin binaries. The test_plugin target drops
# its shared library here and the tests scan it at runtime.
set(PLUGINI_TESTS_PLUGIN_DIR "${CMAKE_BINARY_DIR}/tests/plugins")
add_subdirectory(test_plugin)

list(APPEND CMAKE_MODULE_PATH ${Catch2_SOURCE_DIR}/extras)
include(CTest)
include(Catch)

if(BUILD_TESTING)
    # Collect every *Tests.cpp file and link them into a single test binary.
    # Catch2 aggregates TEST_CASEs from all translation units automatically.
    file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS *Tests.cpp)

    set(test_target plugini_all_tests)
    add_executable(${test_target} ${TEST_SOURCES})
    set_target_properties(${test_target} PROPERTIES CXX_EXTENSIONS OFF)
    target_compile_features(${test_target} PUBLIC cxx_std_23)
    target_link_libraries(${test_target} PRIVATE testMain plugini::plugini)
    target_compile_definitions(${test_target} PRIVATE
        # Use TARGET_FILE_DIR so the path resolves correctly with multi-config
        # generators (Visual Studio, Xcode) that append a per-config subdir.
        PLUGINI_TESTS_PLUGIN_DIR="$<TARGET_FILE_DIR:test_plugin>"
    )
    # Ensure the test plugin is built before the test executable runs.
    add_dependencies(${test_target} test_plugin)
    catch_discover_tests(
        ${test_target}
        TEST_PREFIX ${test_target}.
        REPORTER XML
        OUTPUT_DIR .
        OUTPUT_PREFIX ${test_target}
        OUTPUT_SUFFIX .xml
    )
endif()
