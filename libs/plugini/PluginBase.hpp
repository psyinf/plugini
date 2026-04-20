#pragma once

#include <any>
#include <functional>
#include <string>

// -----------------------------------------------------------------------------
// Symbol visibility / DLL import-export helpers
//
// A plugini plugin is a shared library (DLL / .so / .dylib) that exports a
// small C ABI (`getInfo`, plus whatever else the plugin offers). Plugin
// authors tag those exported functions with one of the macros below.
//
// Usage patterns:
//
//   1. Simple single-plugin case (recommended):
//        // in the plugin's build system:
//        //   target_compile_definitions(my_plugin PRIVATE PLUGINI_PLUGIN_BUILDING)
//        // in the plugin's API header:
//        extern "C" PLUGINI_API void getInfo(plugini::PluginInfo&);
//
//      When the plugin is being built, PLUGINI_API expands to an *export*
//      directive. When the same header is included by code that just wants
//      to consume the plugin via its API header, it expands to *import*.
//
//   2. Per-plugin macro (recommended when a single translation unit may
//      include API headers from more than one plugin):
//        #if defined(MY_PLUGIN_BUILDING)
//            #define MY_PLUGIN_API PLUGINI_EXPORT
//        #else
//            #define MY_PLUGIN_API PLUGINI_IMPORT
//        #endif
// -----------------------------------------------------------------------------

#if defined(_WIN32)
    #define PLUGINI_EXPORT __declspec(dllexport)
    #define PLUGINI_IMPORT __declspec(dllimport)
#elif defined(__GNUC__) || defined(__clang__)
    #define PLUGINI_EXPORT __attribute__((visibility("default")))
    #define PLUGINI_IMPORT __attribute__((visibility("default")))
#else
    #define PLUGINI_EXPORT
    #define PLUGINI_IMPORT
#endif

#if defined(PLUGINI_PLUGIN_BUILDING)
    #define PLUGINI_API PLUGINI_EXPORT
#else
    #define PLUGINI_API PLUGINI_IMPORT
#endif

// Backwards-compatible alias for the previous (export-only) macro.
// Deprecated: prefer PLUGINI_API.
#define PLUGIN_API PLUGINI_EXPORT

namespace plugini
{

struct PluginInfo
{
    std::string name;
    std::string version;

    auto operator<=>(const PluginInfo&) const = default;
};

class PluginBase
{
public:
    using DLLHandle = std::any;

    explicit PluginBase(const std::string& path);
    virtual ~PluginBase();

    PluginBase(const PluginBase&)            = delete;
    PluginBase& operator=(const PluginBase&) = delete;

    void getInfo(PluginInfo& info) const;

    DLLHandle getHandle() const { return dllHandle; }

    static bool hasFunction(DLLHandle handle, const std::string& name)
    {
        return _getFunction(handle, name) != nullptr;
    }

    /**
     * Controls whether the plugin should be unloaded when the operating system unloads the DLL or when the plugin is.
     * This is needed to keep references for the code until all destructors of static objects referencing them in the
     * executable are called.
     * @param delayUnload
     */
    void setDelayUnloadToOperatingSystem(bool delayUnload) { delayUnloadToOperatingSystem = delayUnload; }

protected:
    using Handle = void*;

    void reportMissingInterface(std::string_view path, std::string_view name) const;
    // we require return types (including void) that can be exported from DLL without name mangling (e.g. extern "C")
    template <typename R, typename... Args>
        requires std::is_trivially_copyable_v<R> || std::is_void_v<R>
    std::function<R(Args...)> bindFunction(DLLHandle handle, std::string name)
    {
        if (auto funcPtr = _getFunction(handle, name))
        {
            return reinterpret_cast<R (*)(Args...)>(funcPtr);
        }
        reportMissingInterface(path, name);
        return std::function<R(Args...)>();
    }

private:
    static void* _getFunction(const DLLHandle& handle, std::string_view name);

    DLLHandle                        dllHandle;
    std::string                      path;
    std::function<void(PluginInfo&)> getInfoFunction;
    bool                             delayUnloadToOperatingSystem{false};
};

} // namespace plugini
