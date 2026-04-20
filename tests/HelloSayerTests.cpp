#include "HelloSayer.hpp"
#include "TestPluginClient.hpp"

#include <plugini/PluginManager.hpp>

#include <catch2/catch_test_macros.hpp>

#include <filesystem>
#include <memory>
#include <string>
#include <vector>

TEST_CASE("Plugin hands back HelloSayer instances through a host-owned vector",
          "[plugin][integration][sayer]")
{
    const std::string pluginDir = PLUGINI_TESTS_PLUGIN_DIR;
    REQUIRE(std::filesystem::exists(pluginDir));

    plugini::PluginManager<TestPluginClient, plugini::PluginInfo> manager;
    REQUIRE(manager.scanForPlugins(pluginDir, std::string{plugini::defaultPluginFilter}) >= 1);

    auto plugin = manager.getPlugin("test_plugin");
    REQUIRE(plugin != nullptr);

    std::vector<std::unique_ptr<HelloSayer>> sayers;
    plugin->registerSayers(sayers);

    REQUIRE(sayers.size() == 2);
    REQUIRE(sayers[0] != nullptr);
    REQUIRE(sayers[1] != nullptr);

    CHECK(sayers[0]->say() == "hello from plugin");
    CHECK(sayers[1]->say() == "and another one");

    // Sayers go out of scope here: virtual destructors must dispatch into the
    // plugin. If the base class lacked a virtual dtor this would leak (or
    // crash under sanitizers) — the assertion above keeps the contract honest.
}
