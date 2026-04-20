#pragma once

#include <plugini/StringUtils.hpp>

#include <filesystem>
#include <iostream>
#include <map>
#include <memory>
#include <ranges>
#include <string>
#include <vector>

#include <fmt/core.h>
#include <spdlog/spdlog.h>

namespace plugini {

template <class PluginBaseClass, class PluginInfoType>
class PluginManager
{
    static constexpr bool isDebug()
    {
        bool is_debug = false;
#if defined(_DEBUG) && defined(PLUGIN_MANAGER_USE_DEBUG_SUFFIX)
        is_debug = true;
#endif
        return is_debug;
    };

    using PluginBasePtr = std::shared_ptr<PluginBaseClass>;
    using PluginMap     = std::map<PluginInfoType, PluginBasePtr>;

public:
    PluginManager()          = default;
    virtual ~PluginManager() = default;

    PluginBasePtr makeInstance(const std::filesystem::directory_entry& entry) const
    {
        return std::make_shared<PluginBaseClass>(entry.path().string());
    }

    void scanForPlugins(const std::string& path, const std::vector<std::string>& filters)
    {
        std::ranges::for_each(filters, [this, &path](const auto& filter) { scanForPlugins(path, filter); });
    }

    [[maybe_unused]] size_t scanForPlugins(const std::string& path, const std::string& filter = "*.dll")
    {
        size_t num_loaded = 0;
        spdlog::info(fmt::format("Scanning for plug-ins in : '{}' ", path));

        for (auto& p : std::filesystem::directory_iterator(path)) /*get directory */
        {
            auto file_path       = p.path().filename().string();
            auto file_path_noext = p.path().filename().replace_extension("").string();

            if (!plugini::strings::matchesWildCard(file_path, filter))
            {
                spdlog::debug(fmt::format("Skipping non matching plugin: '{}'", file_path));
                continue;
            }
            else if (!file_path_noext.ends_with("_d") && isDebug())
            {
                spdlog::debug(fmt::format("Skipping non-debug plugin: '{}'", file_path_noext));
                continue;
            }
            else if (file_path_noext.ends_with("_d") && !isDebug())
            {
                spdlog::debug(fmt::format("skipping debug plugin: '{}'", file_path_noext));
                continue;
            }
            else
            {
                try
                {
                    auto           plugin = makeInstance(p);
                    PluginInfoType plugin_info;
                    plugin->getInfo(plugin_info);
                    if (!mPlugins.count(plugin_info))
                    {
                        spdlog::info(fmt::format("Found plugin '{}' ['{}']", file_path, plugin_info.name));
                        mPlugins[plugin_info] = plugin;
                        ++num_loaded;
                    }
                    else
                    {
                        spdlog::debug(fmt::format(
                            "Skipping plugin '{}' ['{}'], already registered.", file_path, plugin_info.name));
                    }
                }
                catch (const std::exception& e)
                {
                    spdlog::error(fmt::format("Error loading plugin '{}': '{}'", p.path().string(), e.what()));
                }
            }
        }
        return num_loaded;
    }

    PluginBasePtr getPlugin(const std::string& plugin_name)
    {
        PluginInfoType plugin_info;
        // plugin info type at least needs a key type string
        plugin_info.name = plugin_name;
        if (!mPlugins.count(plugin_info)) { return nullptr; }
        return mPlugins[plugin_info];
    }

    auto getPluginList() -> PluginMap { return mPlugins; }

    auto getPluginInfos() const -> std::vector<PluginInfoType>
    {
        // use ranges to get the keys from the map
        return mPlugins | std::views::keys | std::ranges::to<std::vector<PluginInfoType>>();
    }

private:
    PluginMap mPlugins;
};

} // namespace plugini
