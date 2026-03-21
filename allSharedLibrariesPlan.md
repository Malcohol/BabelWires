## Plan: Convert All Libraries to Shared Libraries (DRAFT)

This plan converts the entire BabelWires project from static linking to shared libraries. This approach makes every library a shared object, which eliminates by design the singleton duplication, RTTI, and symbol export problems that a mixed static/shared architecture must carefully work around.

Work is organized into small, reversible phases so each PR can land safely. A CMake option provides a fallback to all-static throughout the transition.

### Motivation

Converting all libraries to shared (rather than only plugins) provides:
- **Correct RTTI and `dynamic_cast` across library boundaries** — one `type_info` per type in the process.
- **Singleton consistency by construction** — `IdentifierRegistry` and `DebugLogger` exist exactly once because `BaseLib.so` is loaded once by the dynamic linker.
- **No export-from-executable complexity** — plugins link against `BaseLib.so` / `BabelWiresLib.so` directly instead of requiring `-rdynamic` or `.def` files to expose host symbols.
- **Faster incremental link times** during development.
- **Reduced total binary size** across the executable and test binaries.

### Dependency Graph (Reference)

```
tinyxml2
   │
   ▼
BaseLib
   │
   ├──────────────────────────────────┐
   ▼                                  ▼
BabelWiresLib                     Seq2tapeLib ── sndfile
   │   │
   │   ├── musicLib
   │   │      ├── SmfLib
   │   │      └── TestPluginLib
   │   │
   │   └── libTestDomain
   │
   ├── (+ Qt6, QtNodes)
   ▼
BabelWiresQtUi
   │
   ├── musicLibUi
   ▼
babelWires (exe)
```

All boxes except the executable and external dependencies become shared libraries. External dependencies (`tinyxml2`, `libsndfile`, `nodeeditor`, `googletest`) remain static and are linked into the single BabelWires shared library that consumes them — none of their symbols need to be visible beyond that library's boundary.

---

**Phase 0 — Build-mode switch and CI groundwork (PR 1)**

1. Replace the hard-coded `set(BUILD_SHARED_LIBS OFF)` in [CMakeLists.txt](CMakeLists.txt) with a user-facing option:
   ```cmake
   option(BUILD_SHARED_LIBS "Build all libraries as shared libraries" OFF)
   ```
   Default remains OFF so existing behavior is preserved until the transition is complete.
2. Force external dependencies to build as static libraries regardless of the global setting by adding `set(BUILD_SHARED_LIBS OFF)` inside the existing `block()` scope that wraps extern subdirectories:
   ```cmake
   block(SCOPE_FOR VARIABLES)
       set(BUILD_SHARED_LIBS OFF)
       set(BUILD_TESTING OFF)
       set(tinyxml2_BUILD_TESTING OFF)
       add_subdirectory(extern/tinyxml2)
       include_directories(${tinyxml2_INCLUDE_DIRS})
       add_subdirectory(extern/libsndfile)
       add_subdirectory(extern/nodeeditor)
   endblock()
   ```
   This keeps external static libraries as private implementation details of the BabelWires shared libraries that consume them. Since each external dependency is consumed by exactly one BabelWires library, their symbols do not need to cross any shared-library boundary:
   - `tinyxml2` → absorbed into `BaseLib.so` (hidden by `CXX_VISIBILITY_PRESET hidden`)
   - `libsndfile` → absorbed into `Seq2tapeLib.so`
   - `nodeeditor` → absorbed into `BabelWiresQtUi.so`
   - `googletest` → absorbed into test executables (already static)
3. Ensure `CMAKE_POSITION_INDEPENDENT_CODE ON` remains (already set). This is critical: external static libraries must be built with `-fPIC` so they can be linked into shared libraries.
4. Add a CI matrix entry that builds with `-DBUILD_SHARED_LIBS=ON` alongside the existing static build, so regressions in either mode are caught immediately.
5. Acceptance: both `BUILD_SHARED_LIBS=ON` and `OFF` configure without error; `OFF` produces identical artifacts to today; external deps always produce `.a` files.
6. Rollback: revert option to hard-coded `OFF`.

---

**Phase 1 — Export macros for BaseLib (PR 2)**

1. Create a new header `BaseLib/baseLibExport.hpp` defining a cross-platform export macro:
   ```cpp
   #pragma once
   #if defined(_WIN32) || defined(_WIN64)
     #ifdef BASELIB_EXPORTS
       #define BASELIB_API __declspec(dllexport)
     #else
       #define BASELIB_API __declspec(dllimport)
     #endif
   #else
     #define BASELIB_API __attribute__((visibility("default")))
   #endif
   ```
2. In [BaseLib/CMakeLists.txt](BaseLib/CMakeLists.txt), when `BUILD_SHARED_LIBS` is ON:
   - Set `CXX_VISIBILITY_PRESET hidden` and `VISIBILITY_INLINES_HIDDEN ON` on the `BaseLib` target so only explicitly annotated symbols are exported.
   - Add `BASELIB_EXPORTS` as a private compile definition so the export macro emits `dllexport` when building the library.
3. Annotate all public API classes in BaseLib headers at the **class level** (e.g., `class BASELIB_API Foo { ... };`), which exports all members. This is the fastest way to get a working shared build. Free functions and global variables that form part of the public API also get annotated. Key areas:
   - [BaseLib/Identifiers/identifierRegistry.hpp](BaseLib/Identifiers/identifierRegistry.hpp) — the `IdentifierRegistry` class and its `ReadAccess`/ `WriteAccess` inner classes, `IdentifierRegistryScope`, static members `s_singletonInstance`, `s_mutex`.
   - [BaseLib/Log/debugLogger.hpp](BaseLib/Log/debugLogger.hpp) — `DebugLogger`, `g_debugLogger`, `swapGlobalDebugLogger`, free `logDebug()`.
   - [BaseLib/Serialization/serializable.hpp](BaseLib/Serialization/serializable.hpp) — `Serializable`, `Deserializer`, `Serializer`, `DeserializationRegistryInterface::Entry`, and the `SERIALIZABLE` macro's generated static members.
   - All remaining public headers under `Identifiers/`, `IO/`, `Serialization/`, `Registry/`, `Log/`, `Math/`, `DataContext/`, `Utilities/`, `BlockStream/`, `Cloning/`, `Signal/`, `Hash/`, `Result/`.
   Member-level refinement is deferred to Phase 10.
4. When `BUILD_SHARED_LIBS` is OFF the macro expands to nothing, so static builds are unaffected.
5. Acceptance: `BUILD_SHARED_LIBS=ON` produces `libBaseLib.so` (or `.dylib`/`.dll`); all BaseLib tests link and pass. `BUILD_SHARED_LIBS=OFF` still works.
6. Rollback: remove export header, revert annotations, revert CMake changes.

---

**Phase 2 — Export macros for BabelWiresLib (PR 3)**

1. Create `BabelWiresLib/babelWiresLibExport.hpp` with a `BABELWIRESLIB_API` macro following the same pattern as Phase 1.
2. In [BabelWiresLib/CMakeLists.txt](BabelWiresLib/CMakeLists.txt), set visibility defaults and `BABELWIRESLIB_EXPORTS` private define.
3. Annotate all public API classes at the **class level** (e.g., `class BABELWIRESLIB_API Foo`). Key areas (~130 source files):
   - Type system: `TypeSystem`, `Type`, `TypeConstructor`, and all concrete types.
   - Project model: `Project`, `Node`, `Modifier`, all command classes.
   - Processors: `Processor`, `ProcessorFactory`, `ParallelProcessor`.
   - File formats: `SourceFileFormat`, `TargetFileFormat`.
   - Value tree: `ValueTreeRoot`, `ValueTreeNode`, `EditTree`.
   - Registries: `ProjectContext`, all registry types.
   - Serialization support: all `SERIALIZABLE`-annotated classes.
   - Path: `Path`, `PathStep`.
   Member-level refinement is deferred to Phase 10.
4. Acceptance: `libBabelWiresLib.so` builds and links; all BabelWiresLib tests pass.
5. Rollback: remove export header and annotations.

---

**Phase 3 — Export macros for BabelWiresQtUi and external library adjustments (PR 4)**

1. Create `BabelWiresQtUi/babelWiresQtUiExport.hpp` with `BABELWIRESQTUI_API`.
2. In [BabelWiresQtUi/CMakeLists.txt](BabelWiresQtUi/CMakeLists.txt):
   - Set visibility defaults and `BABELWIRESQTUI_EXPORTS`.
   - Keep the `NODE_EDITOR_STATIC` compile definition: since `nodeeditor` is always built as a static library (forced in Phase 0), this define must remain so that nodeeditor's export macros resolve to nothing. The static `nodeeditor` archive gets linked into `BabelWiresQtUi.so` and its symbols are hidden by the visibility preset.
3. Annotate public API classes at the **class level** in BabelWiresQtUi headers (main window, model bridge, node editor bridge, value editors, value models, context menu, dialogs, utilities). Member-level refinement is deferred to Phase 10.
4. Acceptance: `libBabelWiresQtUi.so` builds; the application starts normally in shared mode; `nodeeditor` symbols are not in `BabelWiresQtUi.so`'s dynamic symbol table.
5. Rollback: remove export annotations.

---

**Phase 4 — Export macros for domain libraries (PR 5)**

1. Create export headers and annotate public symbols for each domain library:

   | Library | Export header | Macro |
   |---------|-------------|-------|
   | `musicLib` | `Domains/Music/MusicLib/musicLibExport.hpp` | `MUSICLIB_API` |
   | `musicLibUi` | `Domains/Music/MusicLibUi/musicLibUiExport.hpp` | `MUSICLIBUI_API` |
   | `SmfLib` | `Domains/Music/Plugins/Smf/Plugin/smfLibExport.hpp` | `SMFLIB_API` |
   | `TestPluginLib` | `Domains/Music/Plugins/TestPlugin/testPluginLibExport.hpp` | `TESTPLUGINLIB_API` |
   | `Seq2tapeLib` | `Domains/Music/Seq2tapeLib/seq2tapeLibExport.hpp` | `SEQ2TAPELIB_API` |
   | `libTestDomain` | `Domains/TestDomain/testDomainExport.hpp` | `TESTDOMAIN_API` |

2. Annotate public API classes at the **class level**. Member-level refinement is deferred to Phase 10.
3. Update each library's `CMakeLists.txt` with visibility defaults and private `*_EXPORTS` define.
4. Acceptance: all domain `.so` files build; domain tests pass.
5. Rollback: remove export headers and annotations per library.

---

**Phase 5 — Test utility libraries and RPATH configuration (PR 6)**

1. Create export headers for test utility libraries (`testUtils`, `libTestUtils`, `seqTestUtils`) if they are built as shared. Alternatively, mark them `STATIC` explicitly since they only exist for testing and need not be shared.
2. Configure RPATH on all executable targets so shared libraries are found at both build time and install time:
   ```cmake
   set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
   set(CMAKE_BUILD_RPATH_USE_ORIGIN TRUE)
   # Or per-target:
   set_target_properties(babelWires PROPERTIES
       BUILD_RPATH "$ORIGIN/../lib"
       INSTALL_RPATH "$ORIGIN/../lib")
   ```
3. On Windows, ensure all `.dll` files end up in the same directory as executables (set `CMAKE_RUNTIME_OUTPUT_DIRECTORY`), or configure `PATH` in CTest.
4. Ensure `gtest_discover_tests()` finds shared libraries at test run time by setting `ENVIRONMENT` or output directories.
5. Acceptance: `ctest` passes all tests in `BUILD_SHARED_LIBS=ON` mode. No "library not found" runtime failures.
6. Rollback: revert RPATH/output-directory changes; static mode unaffected.

---

**Phase 6 — SERIALIZABLE macro and inline static hardening (PR 7)**

1. Audit the `SERIALIZABLE` macro's `inline static const ... s_registryEntry` member:
   - With shared libs, `inline static` variables in header-included templates are guaranteed to have a single instance per type across the process (C++17 inline variable rules + shared linkage). Verify this holds on all target platforms (GCC, Clang, MSVC).
   - If any library is accidentally included via both static and shared paths, the ODR guarantee is violated. Add a CMake check or link-time assertion that prevents mixing.
2. Audit `Detail::SerializableType<T>::s_serializationTag` — same inline-variable concerns.
3. If any edge cases are found, move the static storage into a `.cpp` file with explicit instantiation, or use a registry-based approach instead of inline statics.
4. Acceptance: serialization round-trip tests pass; no duplicate `type_info` or `s_registryEntry` instances detected (verify with `nm -D` / `dumpbin`).
5. Rollback: keep explicit registration as-is; inline statics are harmless in static-linking mode.

---

**Phase 7 — Singleton validation and cleanup (PR 8)**

1. The `IdentifierRegistry` singleton (`s_singletonInstance`, `s_mutex`) in BaseLib is now naturally unique because `BaseLib.so` is loaded once. Remove any workarounds or defensive checks that were only needed because of potential duplication.
2. Same for `DebugLogger::g_debugLogger`.
3. Add startup assertions that confirm exactly one copy of each singleton exists (e.g., compare addresses across library boundaries in a diagnostic test).
4. The `ProjectContext`-based registration pattern (explicit `registerLib()` calls) remains correct and unchanged — it already works cleanly across shared library boundaries because it uses dependency-injected context objects rather than global auto-registration.
5. Acceptance: startup is deterministic; singleton address comparison tests pass.
6. Rollback: re-add defensive checks if singleton uniqueness cannot be guaranteed on a platform.

---

**Phase 8 — Enable runtime-loadable plugins on top of shared core (PR 9)**

1. With all core libraries already shared, converting domain plugins to runtime-loaded modules (`dlopen`/`LoadLibrary`) becomes straightforward:
   - Plugins link against `BaseLib.so` and `BabelWiresLib.so` at build time. The dynamic linker resolves all symbols naturally — no `-rdynamic`, no export lists, no symbol duplication.
   - Each plugin exports a single `registerLib`-style entry point (same pattern already used in every `libRegistration.cpp`).
2. Add a plugin loader (thin wrapper around `dlopen`/`dlsym`/`LoadLibrary`/`GetProcAddress`) in `BabelWiresLib`.
3. Add plugin discovery: scan a configurable directory for `.so`/`.dll`/`.dylib` files, load each, resolve the entry symbol, call it with the `ProjectContext`.
4. Convert `SmfLib` to a runtime-loaded plugin as a pilot: remove its static link from [BabelWiresExe/CMakeLists.txt](BabelWiresExe/CMakeLists.txt), let the loader discover and load it at startup.
5. Acceptance: `SmfLib` loads at runtime; SMF file formats are available; removing the plugin `.so` produces a clear diagnostic rather than a crash.
6. Rollback: re-add static link for `SmfLib`; loader code remains dormant.

---

**Phase 9 — Generalize plugin loading and transition completion (PR 10)**

1. Convert remaining domain plugins (`TestPluginLib`, and any future plugins) to runtime-loaded modules.
2. Separate domain libraries into two categories:
   - **Core domain libraries** (`musicLib`, `musicLibUi`) — remain linked at build time since they provide foundational types that plugins depend on.
   - **Plugins** (`SmfLib`, `TestPluginLib`, future additions) — runtime-loaded, depending on the core domain libraries' shared objects.
3. Set `BUILD_SHARED_LIBS=ON` as the default in [CMakeLists.txt](CMakeLists.txt). Keep `OFF` as a supported fallback for one release cycle.
4. Remove obsolete static-only shims and compatibility code.
5. Update [README.md](README.md) with build instructions for shared mode, plugin development guide, and platform-specific notes (RPATH, DLL paths).
6. Update [TODO.md](TODO.md) to reflect completed migration.
7. Acceptance: shared libraries are the default; all tests pass on all platforms; plugin loading works; static fallback still compiles.
8. Rollback: flip default back to `OFF`.

---

**Phase 10 — Refine export annotations to member level (PR 11)**

Phases 1–4 annotate at the class level (`class BASELIB_API Foo`) to get a working shared build quickly. This over-exports: every member function, static member, and compiler-generated symbol in the class is visible in the dynamic symbol table. Phase 10 narrows exports to only what consumers actually need.

1. For each library, move the `*_API` macro from the class declaration to individual member declarations that form the public API boundary:
   ```cpp
   // Before (class-level, Phases 1–4):
   class BASELIB_API IdentifierRegistry { ... };

   // After (member-level, Phase 10):
   class IdentifierRegistry {
   public:
       BASELIB_API static ReadAccess read();
       BASELIB_API static WriteAccess write();
       BASELIB_API static IdentifierRegistry* swapInstance(IdentifierRegistry*);
       // private/internal members: no annotation
   };
   ```
2. Key guidelines for deciding what to annotate:
   - **Virtual functions**: only the base-class declaration needs export if the vtable is emitted in the library's `.cpp` file. Override declarations in the same library do not need separate annotation.
   - **Inline / header-only methods**: do not need export — they are compiled into the consumer's TU.
   - **Private and internal-only members**: remove export. This reduces symbol table size and prevents accidental coupling.
   - **Template members**: typically header-only and do not need export. Only explicit template instantiations in `.cpp` files need annotation.
   - **Compiler-generated special members** (constructors, destructors, copy/move): export only those that consumers call. If a class is only ever handled via `std::unique_ptr` returned from a factory, only the destructor (implicitly called by the deleter) and the factory function need export.
3. Use `nm -D --defined-only` (Linux) / `dumpbin /EXPORTS` (Windows) before and after to measure the reduction in exported symbols per library.
4. Acceptance: all tests pass; exported symbol counts are reduced; no link failures in consumer libraries or tests.
5. Rollback: revert to class-level annotation (always safe, just over-exports).

---

### Cross-Cutting Concerns

**Export annotation strategy**
- Phases 1–4 use class-level annotation (`class BASELIB_API Foo`) to minimize initial effort and get the shared build working. This exports all members, which is more than necessary but avoids time-consuming per-symbol decisions during the initial migration.
- Phase 10 refines to member-level annotation once the shared build is stable, reducing the exported symbol surface to only what consumers actually need.
- Templated classes/functions that are fully defined in headers generally do not need export annotations (they are instantiated in the consuming TU). Only explicit instantiations in `.cpp` files need export.

**External dependencies — always static**
- All external dependencies are built as static libraries regardless of the global `BUILD_SHARED_LIBS` setting (enforced in Phase 0). Each is absorbed into the single BabelWires shared library that consumes it, and its symbols are not re-exported.
- `tinyxml2`: Static archive linked into `BaseLib.so`. No external symbols leak because `BaseLib` uses `CXX_VISIBILITY_PRESET hidden`.
- `nodeeditor (QtNodes)`: Static archive linked into `BabelWiresQtUi.so`. The `NODE_EDITOR_STATIC` define is kept so nodeeditor's own export macros resolve to nothing.
- `libsndfile`: Static archive linked into `Seq2tapeLib.so`. Used only internally for audio I/O.
- `googletest`: Static, linked into test executables. `gtest_force_shared_crt` handles MSVC CRT.
- This approach avoids shipping extra shared libraries for external code, simplifies deployment, and avoids needing to manage version compatibility of third-party `.so`/`.dll` files.

**Template-heavy code**
- Templates instantiated in consumer TUs do not need export. Only symbols with out-of-line definitions in `.cpp` files need `*_API`.
- Explicit template instantiations in `.cpp` files must be exported.
- The `Instance/` template headers (e.g., `arrayTypeInstance.hpp`, `recordWithVariantsTypeInstance.hpp`) are header-only and should not require annotation.

**macOS considerations**
- Use `@rpath` in install names; CMake handles this with `MACOSX_RPATH ON` (default since CMake 3.0).
- `__attribute__((visibility("default")))` works identically to Linux.

### Verification

- **Build**: `cmake -DBUILD_SHARED_LIBS=ON ..` followed by `make` / `ninja` must produce `.so`/`.dll`/`.dylib` for every library target.
- **Test**: `ctest` must pass all tests in both shared and static modes.
- **Symbol check**: `nm -D` (Linux) / `dumpbin /EXPORTS` (Windows) on each shared library to verify only intended symbols are exported (not internal implementation details).
- **Singleton check**: A diagnostic test loads two different shared libraries and confirms `IdentifierRegistry::s_singletonInstance` has the same address in both.
- **RTTI check**: A diagnostic test confirms `dynamic_cast` works correctly across library boundaries for key polymorphic hierarchies (`Type`, `Processor`, `SourceFileFormat`, etc.).
- **Runtime plugin check** (Phase 8+): start with no plugins in the plugin directory → verify clean startup with diagnostics; add SmfLib plugin → verify SMF formats are registered.

### PR Slicing Guidance

- Phases 1–4 (class-level export macros) are the bulk of the initial mechanical work. Within each phase, annotation can be split further by subdirectory if the PR is too large.
- Phase 5 (RPATH) is largely independent and can be done in parallel with later annotation phases.
- Phases 8–9 (runtime plugin loading) are optional extensions that leverage the all-shared foundation but are not required for the core migration.
- Phase 10 (member-level refinement) can be done incrementally, one library at a time, and is not blocking for any other phase.
- Enforce "shippable at each step": `BUILD_SHARED_LIBS=OFF` must remain green until Phase 9 flips the default.
- Require explicit rollback note in each PR description.
