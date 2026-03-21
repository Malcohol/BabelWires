## Phase 8 — Enable runtime-loadable plugins on top of shared core (PR 9)

### Goal

With all core libraries already shared, add infrastructure for runtime-loaded plugin modules and pilot with SmfLib.

### Background

Because all core libraries are shared, plugins can link against `BaseLib.so` and `BabelWiresLib.so` at build time. The dynamic linker resolves all symbols naturally — no `-rdynamic`, no export lists, no symbol duplication. Each plugin exports a single `registerLib`-style entry point (same pattern already used in every `libRegistration.cpp`).

### Changes required

#### 1. Plugin loader

Add a plugin loader in `BabelWiresLib` — a thin wrapper around `dlopen`/`dlsym` (Linux/macOS) and `LoadLibrary`/`GetProcAddress` (Windows).

#### 2. Plugin discovery

Scan a configurable directory for `.so`/`.dll`/`.dylib` files, load each, resolve the entry symbol, and call it with the `ProjectContext`.

#### 3. Pilot: SmfLib as runtime-loaded plugin

Convert `SmfLib` to a runtime-loaded plugin:
- Remove its static link from [BabelWiresExe/CMakeLists.txt](BabelWiresExe/CMakeLists.txt).
- Let the loader discover and load it at startup.
- Ensure `SmfLib` links against `BaseLib.so`, `BabelWiresLib.so`, and `musicLib.so` at build time.

### Acceptance criteria

- `SmfLib` loads at runtime; SMF file formats are available.
- Removing the plugin `.so` produces a clear diagnostic rather than a crash.

### Rollback

Re-add static link for `SmfLib`; loader code remains dormant.
