#pragma once

#include <plugini/PluginBase.hpp>

#include <functional>
#include <string>

// Test-side wrapper around a loaded test_plugin shared library.
class TestPluginClient : public plugini::PluginBase
{
public:
    explicit TestPluginClient(const std::string& path)
      : plugini::PluginBase(path)
    {
        addFn    = bindFunction<int, int, int>(getHandle(), "add");
        answerFn = bindFunction<int>(getHandle(), "answer");
    }

    int add(int a, int b) const { return addFn ? addFn(a, b) : 0; }

    int answer() const { return answerFn ? answerFn() : 0; }

private:
    std::function<int(int, int)> addFn;
    std::function<int()>         answerFn;
};
