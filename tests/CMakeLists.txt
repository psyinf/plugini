project(plugini_tests LANGUAGES CXX)

add_library(testMain OBJECT testMain.cpp)
target_link_libraries(testMain PUBLIC Catch2::Catch2)

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
