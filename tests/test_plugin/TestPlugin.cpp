#define PLUGINI_TEST_PLUGIN_BUILDING
#include "TestPluginApi.hpp"

extern "C" {

void getInfo(plugini::PluginInfo& info)
{
    info.name    = "test_plugin";
    info.version = "0.1.0";
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
