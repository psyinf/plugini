#pragma once

#include <plugini/PluginBase.hpp>

#include <string>

// Host-side wrapper around a loaded hello_plugin DLL.
// Derives from plugini::PluginBase so it can use the protected bindFunction<>()
// helper to resolve exported C symbols.
class HelloPluginClient : public plugini::PluginBase
{
public:
    explicit HelloPluginClient(const std::string& path)
      : plugini::PluginBase(path)
    {
        sayHelloFn = bindFunction<void>(getHandle(), "sayHello");
        addFn      = bindFunction<int, int, int>(getHandle(), "add");
    }

    void sayHello() const
    {
        if (sayHelloFn) { sayHelloFn(); }
    }

    int add(int a, int b) const
    {
        return addFn ? addFn(a, b) : 0;
    }

private:
    std::function<void()>            sayHelloFn;
    std::function<int(int, int)>     addFn;
};
