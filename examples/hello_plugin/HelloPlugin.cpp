#define PLUGINI_HELLO_BUILDING
#include "HelloPluginApi.hpp"

#include <cstdio>

extern "C" {

void getInfo(plugini::PluginInfo& info)
{
    info.name    = "hello_plugin";
    info.version = "1.0.0";
}

void sayHello()
{
    std::printf("[hello_plugin] Hello, world from the plugin DLL!\n");
}

int add(int a, int b)
{
    return a + b;
}

} // extern "C"
