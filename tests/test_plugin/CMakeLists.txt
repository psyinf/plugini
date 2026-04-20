project(test_plugin)

add_library(${PROJECT_NAME} SHARED
    TestPlugin.cpp
    TestPluginApi.hpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE plugini::plugini)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/..    # access to HelloSayer.hpp shared with the tests
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN ON
    PREFIX ""    # drop the "lib" prefix on Unix, so wildcard "*.dll"/"*.so" filters are uniform
)

# Drop the DLL/so in a stable directory the tests can scan at runtime.
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${PLUGINI_TESTS_PLUGIN_DIR}"
    LIBRARY_OUTPUT_DIRECTORY "${PLUGINI_TESTS_PLUGIN_DIR}"
)
