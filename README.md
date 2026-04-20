# plugini

A tiny, header-light C++23 plugin/DLL loading library. Provides:

- `plugini::PluginBase` — RAII wrapper around a shared library (`LoadLibraryEx` / `dlopen`) with type-safe
  `bindFunction<R, Args...>()` helper for C-exported symbols.
- `plugini::PluginManager<Base, Info>` — scans a directory, instantiates plugins matching a wildcard filter,
  calls `getInfo`, and deduplicates by `PluginInfo`.

Structure is modeled after [psyinf/prototools](https://github.com/psyinf/prototools): standalone CMake project,
CPM-managed dependencies (`fmt`, `spdlog`, `Catch2`), and a `libs/` + `tests/` layout.

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
| `PLUGINI_INSTALL`        | `ON`*   | Generate `install()` rules            |
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

## License

TBD — add a `LICENSE` file before publishing.
