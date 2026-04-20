#pragma once

#include <string>

// Abstract base class shared between the test host and the test plugin.
// The plugin derives from this privately (anonymous/local class) and hands
// instances back to the host via a std::vector<std::unique_ptr<HelloSayer>>&
// passed through the plugin API.
//
// A virtual destructor is mandatory: the owning unique_ptr lives in the host
// but the concrete type lives in the plugin, so deletion must dispatch through
// the vtable.
class HelloSayer
{
public:
    virtual ~HelloSayer()             = default;
    virtual std::string say() const   = 0;
};
