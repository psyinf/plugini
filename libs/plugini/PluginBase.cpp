#include "PluginBase.hpp"

#include <array>
#include <fmt/core.h>
#include <spdlog/spdlog.h>
#include <functional>
#include <stdexcept>
#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <Windows.h>
#elif __linux__
    #include <dlfcn.h>
#endif

using namespace plugini;

#ifdef _WIN32
static std::string FormatErrorMessage(const DWORD errorCode)
{
    std::array<char, 512> message{};

    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                  nullptr,
                  errorCode,
                  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                  message.data(),
                  static_cast<DWORD>(message.size()),
                  nullptr);

    return std::string(message.data());
}
#endif

// #TODO: explore modes and try to match flags to unify interface here

static std::string getLastErrorString()
{
#if defined(_MSC_VER) // Microsoft compiler
    DWORD error        = ::GetLastError();
    auto  last_err_str = FormatErrorMessage(error);
    last_err_str.append(" ( ");
    last_err_str.append(std::to_string(error));
    last_err_str.append(" )");
    return last_err_str;
#elif __linux__
    char* error = dlerror();
    return error ? std::string(error) : std::string();
#endif
}

plugini::PluginBase::PluginBase(const std::string& path)
  : path(path)
{
    // Keep the platform-specific OS handle local so we can simply check it
    // against 0/nullptr; it is wrapped into the DLLHandle (std::any) only
    // once the load has succeeded.
#if defined(_WIN32)
    HMODULE handle = LoadLibraryEx(path.data(), nullptr, 0x0);
#elif defined(__linux__)
    void*   handle = dlopen(path.data(), RTLD_NOW);
#endif

    if (!handle)
    {
        auto last_err_str = getLastErrorString();
        throw std::invalid_argument(fmt::format("Could not load '{}'.\nError reported: {}", path, last_err_str));
    }

    // TODO: move to generic list or use introspection
    getInfoFunction = bindFunction<void, PluginInfo&>(handle, std::string("getInfo"));
    if (!getInfoFunction) { reportMissingInterface(path, "getInfo"); }

    dllHandle = handle;
}

void plugini::PluginBase::reportMissingInterface(std::string_view path, std::string_view name) const
{
    throw std::invalid_argument(
        fmt::format("Plugin at '{}' is not a valid plugin (Missing '{}' interface)", path, name));
}

void plugini::PluginBase::getInfo(PluginInfo& info) const
{
    getInfoFunction(info);
}

plugini::PluginBase::~PluginBase()
{
    if (dllHandle.has_value())
    {
        PluginInfo info;
        getInfo(info);
        if (delayUnloadToOperatingSystem)
        {
            spdlog::debug("Unloading plugin '{}' delayed to operating system\n", info.name);
            return;
        }
        else
        {
            spdlog::debug("Unloading plugin '{}'\n", info.name);

#ifdef _WIN32
            ::FreeLibrary(std::any_cast<HMODULE>(dllHandle));
#elif __linux__
            dlclose(std::any_cast<void*>(dllHandle));
#endif
            dllHandle.reset();
        }
    }
}

void* plugini::PluginBase::_getFunction(const DLLHandle& handle, std::string_view name)
{
#if defined(_MSC_VER) // Microsoft compiler
    return ::GetProcAddress(std::any_cast<HMODULE>(handle), name.data());
#elif __linux__
    return dlsym(std::any_cast<void*>(handle), name.data());
#endif
}
