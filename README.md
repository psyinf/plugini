# plugini

A tiny, header-light C++23 plugin/DLL loading library. Provides:

- `plugini::PluginBase` — RAII wrapper around a shared library (`LoadLibraryEx` / `dlopen`) with type-safe
  `bindFunction<R, Args...>()` helper for C-exported symbols.
- `plugini::PluginManager<Base, Info>` — scans a directory, instantiates plugins matching a wildcard filter,
  calls `getInfo`, and deduplicates by `PluginInfo`.

## Building

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

Standalone build options:

| Option                   | Default | Description                           |
| ------------------------ | ------- | ------------------------------------- |
| `PLUGINI_ENABLE_TESTING` | `ON`    | Build Catch2-based unit tests         |
| `PLUGINI_BUILD_EXAMPLES` | `ON`*   | Build the `examples/` host + plugin   |
| `ENABLE_COVERAGE`        | `OFF`   | GCC/Clang `--coverage` instrumentation |

\* Only `ON` by default when built as the top-level (standalone) project.

## Embedding

Add as a subdirectory (e.g. via CPM, `FetchContent`, or a git submodule):

```cmake
CPMAddPackage(
    NAME plugini
    GITHUB_REPOSITORY <your-org>/plugini
    GIT_TAG main
)

target_link_libraries(my_app PRIVATE plugini::plugini)
```

When embedded, the `apps/` and `tests/` subdirectories and install rules are skipped automatically.

## Usage

```cpp
#include <plugini/PluginBase.hpp>
#include <plugini/PluginManager.hpp>

struct MyInfo : plugini::PluginInfo { /* extend as needed */ };

class MyPlugin : public plugini::PluginBase
{
public:
    using PluginBase::PluginBase;
    // bind additional exported functions in the constructor via bindFunction<...>()
};

plugini::PluginManager<MyPlugin, MyInfo> manager;
manager.scanForPlugins("./plugins", "*.dll");
```

Each plugin shared library must export at minimum a C function:

```cpp
#include <plugini/PluginBase.hpp>

extern "C" PLUGINI_API void getInfo(plugini::PluginInfo& info) {
    info.name    = "example";
    info.version = "1.0.0";
}
```

## Exporting symbols from a plugin

`<plugini/PluginBase.hpp>` provides three macros for tagging exported functions
so the right `__declspec(dll...)` / visibility attribute is emitted on every
supported platform:

| Macro            | Expands to                                                           |
| ---------------- | -------------------------------------------------------------------- |
| `PLUGINI_EXPORT` | `__declspec(dllexport)` on MSVC, `__attribute__((visibility("default")))` elsewhere |
| `PLUGINI_IMPORT` | `__declspec(dllimport)` on MSVC, `__attribute__((visibility("default")))` elsewhere |
| `PLUGINI_API`    | `PLUGINI_EXPORT` when `PLUGINI_PLUGIN_BUILDING` is defined, else `PLUGINI_IMPORT` |

Recommended pattern for a single plugin:

```cmake
# plugin/CMakeLists.txt
add_library(my_plugin SHARED my_plugin.cpp)
target_link_libraries(my_plugin PRIVATE plugini::plugini)
target_compile_definitions(my_plugin PRIVATE PLUGINI_PLUGIN_BUILDING)
```

```cpp
// plugin/my_plugin_api.hpp — included by the plugin and (optionally) by any host
// that wants to link through the header instead of GetProcAddress.
#include <plugini/PluginBase.hpp>

extern "C" PLUGINI_API void getInfo(plugini::PluginInfo&);
extern "C" PLUGINI_API int  doWork(int);
```

If a single translation unit may include API headers from more than one plugin,
give each plugin its own macro pair instead of using the shared `PLUGINI_API`:

```cpp
#if defined(MY_PLUGIN_BUILDING)
    #define MY_PLUGIN_API PLUGINI_EXPORT
#else
    #define MY_PLUGIN_API PLUGINI_IMPORT
#endif
```

## Example

A runnable end-to-end example lives under `examples/`:

- `examples/hello_plugin/` — a tiny shared library that exports `getInfo`,
  `sayHello` and `add(int, int)`.
- `examples/host/` — an executable that uses `plugini::PluginManager` to scan a
  directory, register any plugins it finds, then calls `sayHello()` / `add()` on
  the registered `hello_plugin`.

Build and run:

```bash
cmake -S . -B build
cmake --build build
./build/examples/host/hello_host
```

The host is configured at build time with the path to
`build/examples/plugins/`, where the example DLL is placed, so it works with no
arguments. You can also pass a different directory as the first argument.

## Debug vs. release plugins (`_d` suffix convention)

Debug and release binaries usually have an incompatible C++ ABI (different
iterator debug levels, different `_ITERATOR_DEBUG_LEVEL`, different STL
layouts, etc.). Loading a release plugin into a debug host — or vice versa —
will silently corrupt memory.

`PluginManager` supports a simple convention to let debug and release plugins
coexist in a single directory:

- Debug plugins must have a file stem ending in `_d`, e.g. `my_plugin_d.dll`.
- Release plugins must **not** end in `_d`, e.g. `my_plugin.dll`.

At scan time the manager behaves as follows:

| Host build                                      | `my_plugin.dll` | `my_plugin_d.dll` |
| ----------------------------------------------- | --------------- | ----------------- |
| Release (default)                               | ✅ loaded       | ❌ skipped        |
| Debug + `-DPLUGIN_MANAGER_USE_DEBUG_SUFFIX`     | ❌ skipped      | ✅ loaded         |

Enable the debug-side behavior with:

```cmake
target_compile_definitions(my_host PRIVATE
    $<$<CONFIG:Debug>:PLUGIN_MANAGER_USE_DEBUG_SUFFIX>
)
```

Make sure your debug plugin's CMake target also appends the `_d` suffix, e.g.
`set_target_properties(my_plugin PROPERTIES DEBUG_POSTFIX "_d")`.

## Passing C++ types across the plugin boundary

The plugin ABI is `extern "C"` only for **symbol names** — it does not convert
C++ types to anything C-compatible. Functions may still take and return C++
types (references, `std::vector`, `std::unique_ptr`, polymorphic base
classes, …). This works as long as the host and the plugin are built with the
same compiler, standard library, CRT and build configuration.

A working pattern (see `tests/HelloSayerTests.cpp`):

```cpp
// shared header (host + plugin both include this)
class HelloSayer {
public:
    virtual ~HelloSayer() = default;          // REQUIRED: dtor must be virtual
    virtual std::string say() const = 0;
};
```

```cpp
// plugin: a local derived class is never named outside the DLL
namespace {
class PluginSayer : public HelloSayer {
    std::string say() const override { return "hello from plugin"; }
};
} // namespace

extern "C" PLUGINI_API
void registerSayers(std::vector<std::unique_ptr<HelloSayer>>& sayers) {
    sayers.emplace_back(std::make_unique<PluginSayer>());
}
```

```cpp
// host side
std::vector<std::unique_ptr<HelloSayer>> sayers;
plugin->registerSayers(sayers);               // plugin appends its instance
// ~unique_ptr<HelloSayer>() dispatches through the virtual dtor back
// into the plugin's ~PluginSayer().
```

Things to watch out for:

- **Virtual destructor.** The owning `unique_ptr` lives in the host, but the
  concrete type lives in the plugin; deletion must go through the vtable.
- **No mixing compilers / CRTs.** MSVC vs. Clang, `/MT` vs. `/MD`, libstdc++
  vs. libc++ — any of these mismatched between host and plugin is undefined
  behavior for types like `std::vector` or `std::string`.
- **Same build configuration.** Do not mix a Release host with a Debug
  plugin; see the `_d` suffix section above.
- **Plugin lifetime.** Keep the `PluginBase` instance alive as long as any
  object it handed out is still referenced, otherwise the DLL may be unloaded
  under the object's feet.

## License

Released under the [MIT License](LICENSE).
