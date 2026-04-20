project(hello_host)

add_executable(${PROJECT_NAME}
    main.cpp
    HelloPluginClient.hpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE plugini::plugini)

# Pass the default plugin directory as a compile definition, so the host can
# scan it out-of-the-box with no arguments.
target_compile_definitions(${PROJECT_NAME} PRIVATE
    PLUGINI_EXAMPLES_DEFAULT_PLUGIN_DIR="${PLUGINI_EXAMPLES_PLUGIN_DIR}"
)

# Ensure the plugin DLL is built before the host so a fresh run has something to load.
add_dependencies(${PROJECT_NAME} hello_plugin)
