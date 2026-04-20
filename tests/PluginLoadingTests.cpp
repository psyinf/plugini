#include "TestPluginClient.hpp"

#include <plugini/PluginManager.hpp>

#include <catch2/catch_test_macros.hpp>

#include <filesystem>
#include <string>

namespace {

constexpr const char* kPluginFilter =
#if defined(_WIN32)
    "*.dll";
#elif defined(__APPLE__)
    "*.dylib";
#else
    "*.so";
#endif

} // namespace

TEST_CASE("PluginManager loads a freshly built shared-library plugin", "[plugin][integration]")
{
    const std::string pluginDir = PLUGINI_TESTS_PLUGIN_DIR;

    REQUIRE(std::filesystem::exists(pluginDir));

    plugini::PluginManager<TestPluginClient, plugini::PluginInfo> manager;
    const auto loaded = manager.scanForPlugins(pluginDir, kPluginFilter);

    REQUIRE(loaded >= 1);

    auto plugin = manager.getPlugin("test_plugin");
    REQUIRE(plugin != nullptr);

    CHECK(plugin->add(2, 40) == 42);
    CHECK(plugin->answer() == 42);
}
