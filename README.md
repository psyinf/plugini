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
extern "C" PLUGIN_API void getInfo(plugini::PluginInfo& info) {
    info.name    = "example";
    info.version = "1.0.0";
}
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
