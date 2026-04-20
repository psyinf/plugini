#include "HelloPluginClient.hpp"

#include <plugini/PluginManager.hpp>

#include <spdlog/spdlog.h>

#include <cstdlib>
#include <filesystem>
#include <string>

int main(int argc, char* argv[])
{
    spdlog::set_level(spdlog::level::debug);

    // The host is configured at build time with a default plugin directory
    // (see examples/CMakeLists.txt). Allow overriding it via argv[1] for ad-hoc runs.
    std::string pluginDir = (argc > 1) ? argv[1] : PLUGINI_EXAMPLES_DEFAULT_PLUGIN_DIR;

    if (!std::filesystem::exists(pluginDir))
    {
        spdlog::error("Plugin directory does not exist: {}", pluginDir);
        return EXIT_FAILURE;
    }

    plugini::PluginManager<HelloPluginClient, plugini::PluginInfo> manager;
    const auto loaded = manager.scanForPlugins(pluginDir, std::string{plugini::defaultPluginFilter});
    spdlog::info("Registered {} plugin(s)", loaded);

    // Now that plugins are registered, look one up by name and invoke its exports.
    if (auto plugin = manager.getPlugin("hello_plugin"))
    {
        plugin->sayHello();
        spdlog::info("hello_plugin::add(2, 40) = {}", plugin->add(2, 40));
        return EXIT_SUCCESS;
    }

    spdlog::error("hello_plugin was not found in '{}'", pluginDir);
    return EXIT_FAILURE;
}
