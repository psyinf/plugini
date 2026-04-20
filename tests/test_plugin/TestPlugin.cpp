#define PLUGINI_TEST_PLUGIN_BUILDING
#include "TestPluginApi.hpp"

#include <memory>
#include <string>

namespace {

// Local (anonymous-namespace) derived type: the host only ever knows about
// it through the HelloSayer base-class interface.
class PluginSayer : public HelloSayer
{
public:
    explicit PluginSayer(std::string message)
      : mMessage(std::move(message))
    {
    }

    std::string say() const override { return mMessage; }

private:
    std::string mMessage;
};

} // namespace

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

void registerSayers(std::vector<std::unique_ptr<HelloSayer>>& sayers)
{
    sayers.emplace_back(std::make_unique<PluginSayer>("hello from plugin"));
    sayers.emplace_back(std::make_unique<PluginSayer>("and another one"));
}

} // extern "C"
