#pragma once

#include <any>
#include <functional>
#include <string>

#ifdef _WIN32
    #define PLUGIN_API __declspec(dllexport)
#elif __linux__
    #define PLUGIN_API
#endif

namespace plugini {

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
