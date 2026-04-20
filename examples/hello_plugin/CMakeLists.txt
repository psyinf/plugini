project(hello_plugin)

add_library(${PROJECT_NAME} SHARED
    HelloPlugin.cpp
    HelloPluginApi.hpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE plugini::plugini)

# Tell <plugini/PluginBase.hpp> that this translation unit is *building* the
# plugin, so PLUGINI_API expands to an export directive. Consumers of the
# header (tests, other hosts) do not set this and get an import directive.
target_compile_definitions(${PROJECT_NAME} PRIVATE PLUGINI_PLUGIN_BUILDING)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

# Non-Windows platforms: hide everything by default, export only what's tagged.
set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN ON
    PREFIX ""    # drop the "lib" prefix on Unix, so wildcard "*.dll"/"*.so" filters are uniform
)

# Drop the DLL/so next to its siblings in a single "plugins" folder.
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${PLUGINI_EXAMPLES_PLUGIN_DIR}"
    LIBRARY_OUTPUT_DIRECTORY "${PLUGINI_EXAMPLES_PLUGIN_DIR}"
)
