#pragma once

// Shared C ABI between the test plugin DLL and the test host.

#if defined(_WIN32)
    #if defined(PLUGINI_TEST_PLUGIN_BUILDING)
        #define PLUGINI_TEST_PLUGIN_API __declspec(dllexport)
    #else
        #define PLUGINI_TEST_PLUGIN_API __declspec(dllimport)
    #endif
#else
    #define PLUGINI_TEST_PLUGIN_API __attribute__((visibility("default")))
#endif

#include <plugini/PluginBase.hpp>

#include <memory>
#include <vector>

#include "HelloSayer.hpp"

extern "C" {

// Required by plugini::PluginBase: fills in name/version.
PLUGINI_TEST_PLUGIN_API void getInfo(plugini::PluginInfo& info);

// Test entry points.
PLUGINI_TEST_PLUGIN_API int  add(int a, int b);
PLUGINI_TEST_PLUGIN_API int  answer();

// Appends plugin-owned HelloSayer instances to a host-provided vector.
// Demonstrates passing a C++ type by reference across the DLL boundary and
// transferring ownership of polymorphic objects via std::unique_ptr.
PLUGINI_TEST_PLUGIN_API void registerSayers(std::vector<std::unique_ptr<HelloSayer>>& sayers);

} // extern "C"
