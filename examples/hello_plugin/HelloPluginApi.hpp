#pragma once

// Shared C ABI between a plugini plugin DLL and any code that wants to link
// against it through this header. The DLL's build system defines
// PLUGINI_PLUGIN_BUILDING on the plugin target, so PLUGINI_API resolves to an
// *export* there; everywhere else it resolves to *import* (or to a default
// visibility attribute on non-Windows).

#include <plugini/PluginBase.hpp> // provides PLUGINI_API and plugini::PluginInfo

extern "C" {

// Required by plugini::PluginBase: fills in name/version.
PLUGINI_API void getInfo(plugini::PluginInfo& info);

// The hello-world entry point we want to call after loading.
PLUGINI_API void sayHello();

// Example of a function returning a trivially-copyable value.
PLUGINI_API int add(int a, int b);

} // extern "C"
