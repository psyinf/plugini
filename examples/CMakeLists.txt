# Put every example plugin DLL into a single "plugins" directory next to the
# host executable, so the host has a stable location to scan at runtime.
set(PLUGINI_EXAMPLES_PLUGIN_DIR "${CMAKE_BINARY_DIR}/examples/plugins")

add_subdirectory(hello_plugin)
add_subdirectory(host)
