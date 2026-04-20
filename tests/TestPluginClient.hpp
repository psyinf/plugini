#pragma once

#include "HelloSayer.hpp"

#include <plugini/PluginBase.hpp>

#include <functional>
#include <memory>
#include <string>
#include <vector>

// Test-side wrapper around a loaded test_plugin shared library.
class TestPluginClient : public plugini::PluginBase
{
public:
    explicit TestPluginClient(const std::string& path)
      : plugini::PluginBase(path)
    {
        addFn             = bindFunction<int, int, int>(getHandle(), "add");
        answerFn          = bindFunction<int>(getHandle(), "answer");
        registerSayersFn  = bindFunction<void, std::vector<std::unique_ptr<HelloSayer>>&>(
            getHandle(), "registerSayers");
    }

    int add(int a, int b) const { return addFn ? addFn(a, b) : 0; }

    int answer() const { return answerFn ? answerFn() : 0; }

    void registerSayers(std::vector<std::unique_ptr<HelloSayer>>& sayers) const
    {
        if (registerSayersFn) { registerSayersFn(sayers); }
    }

private:
    std::function<int(int, int)>                                    addFn;
    std::function<int()>                                            answerFn;
    std::function<void(std::vector<std::unique_ptr<HelloSayer>>&)>  registerSayersFn;
};
