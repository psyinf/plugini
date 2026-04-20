#define PLUGINI_TEST_PLUGIN_BUILDING
#include "TestPluginApi.hpp"

extern "C" {

void getInfo(plugini::PluginInfo& info)
{
    info.name    = "test_plugin";
    // NOTE: PluginManager::getPlugin(name) compares full PluginInfo including
    // version, so we leave it empty to allow lookup by name alone.
    info.version = "";
}

int add(int a, int b)
{
    return a + b;
}

int answer()
{
    return 42;
}

} // extern "C"
