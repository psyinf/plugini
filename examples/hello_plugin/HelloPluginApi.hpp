#pragma once

// Shared C ABI between a plugini plugin DLL and the host.
// Both sides include this header; the DLL compiles with PLUGINI_HELLO_BUILDING
// defined so that symbols are exported.

#if defined(_WIN32)
    #if defined(PLUGINI_HELLO_BUILDING)
        #define PLUGINI_HELLO_API __declspec(dllexport)
    #else
        #define PLUGINI_HELLO_API __declspec(dllimport)
    #endif
#else
    #define PLUGINI_HELLO_API __attribute__((visibility("default")))
#endif

#include <plugini/PluginBase.hpp> // for plugini::PluginInfo

extern "C" {

// Required by plugini::PluginBase: fills in name/version.
PLUGINI_HELLO_API void getInfo(plugini::PluginInfo& info);

// The hello-world entry point we want to call after loading.
PLUGINI_HELLO_API void sayHello();

// Example of a function returning a trivially-copyable value.
PLUGINI_HELLO_API int add(int a, int b);

} // extern "C"
