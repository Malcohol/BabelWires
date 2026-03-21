## Phase 1 — Export macros for BaseLib (PR 2)

### Goal

Make `BaseLib` build as a shared library when `BUILD_SHARED_LIBS=ON` by adding a cross-platform export macro and annotating all public API classes.

### Changes required

#### 1. Create export header

Create `BaseLib/baseLibExport.hpp`:
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

#### 2. Update CMakeLists.txt

In [BaseLib/CMakeLists.txt](BaseLib/CMakeLists.txt), when `BUILD_SHARED_LIBS` is ON:
- Set `CXX_VISIBILITY_PRESET hidden` and `VISIBILITY_INLINES_HIDDEN ON` on the `BaseLib` target so only explicitly annotated symbols are exported.
- Add `BASELIB_EXPORTS` as a private compile definition so the export macro emits `dllexport` when building the library.

```cmake
if(BUILD_SHARED_LIBS)
    set_target_properties(BaseLib PROPERTIES
        CXX_VISIBILITY_PRESET hidden
        VISIBILITY_INLINES_HIDDEN ON)
    target_compile_definitions(BaseLib PRIVATE BASELIB_EXPORTS)
endif()
```

#### 3. Annotate public API classes

Annotate all public API classes in BaseLib headers at the **class level** (e.g., `class BASELIB_API Foo { ... };`), which exports all members. This is the fastest way to get a working shared build. Free functions and global variables that form part of the public API also get annotated.

Key areas:
- [BaseLib/Identifiers/identifierRegistry.hpp](BaseLib/Identifiers/identifierRegistry.hpp) — the `IdentifierRegistry` class and its `ReadAccess` / `WriteAccess` inner classes, `IdentifierRegistryScope`, static members `s_singletonInstance`, `s_mutex`.
- [BaseLib/Log/debugLogger.hpp](BaseLib/Log/debugLogger.hpp) — `DebugLogger`, `g_debugLogger`, `swapGlobalDebugLogger`, free `logDebug()`.
- [BaseLib/Serialization/serializable.hpp](BaseLib/Serialization/serializable.hpp) — `Serializable`, `Deserializer`, `Serializer`, `DeserializationRegistryInterface::Entry`, and the `SERIALIZABLE` macro's generated static members.
- All remaining public headers under `Identifiers/`, `IO/`, `Serialization/`, `Registry/`, `Log/`, `Math/`, `DataContext/`, `Utilities/`, `BlockStream/`, `Cloning/`, `Signal/`, `Hash/`, `Result/`.

Member-level refinement is deferred to [Phase 10](allSharedPhase10.md).

#### 4. Static-build compatibility

When `BUILD_SHARED_LIBS` is OFF, the macro expands to nothing (no `BASELIB_EXPORTS` defined, visibility attributes are not set), so static builds are unaffected.

### Acceptance criteria

- `BUILD_SHARED_LIBS=ON` produces `libBaseLib.so` (or `.dylib`/`.dll`); all BaseLib tests link and pass.
- `BUILD_SHARED_LIBS=OFF` still works identically to before.

### Rollback

Remove export header, revert annotations, revert CMake changes.
