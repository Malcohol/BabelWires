## Phase 0 — Build-mode switch and CI groundwork (PR 1)

### Goal

Introduce a `BUILD_SHARED_LIBS` option while keeping static as the default. Force external dependencies to remain static so they are absorbed into the BabelWires shared libraries that consume them.

### Changes required

All changes are in the top-level [CMakeLists.txt](CMakeLists.txt).

#### 1. Replace hard-coded `BUILD_SHARED_LIBS OFF` with a user-facing option

The current code (line 19–20):
```cmake
# Lots of work will be required to support shared libs. Waiting to see whether C++ Modules will help.
set(BUILD_SHARED_LIBS OFF)
```

Replace with:
```cmake
option(BUILD_SHARED_LIBS "Build all BabelWires libraries as shared libraries" OFF)
```

Default remains OFF so existing behavior is preserved.

#### 2. Force external dependencies to build as static libraries

The existing `block()` scope (lines 67–74) currently only overrides `BUILD_TESTING`:
```cmake
# Disable BUILD_TESTING for external libraries.
block(SCOPE_FOR VARIABLES)
    set(BUILD_TESTING OFF)
    set(tinyxml2_BUILD_TESTING OFF)
    add_subdirectory(extern/tinyxml2)
    INCLUDE_DIRECTORIES(${tinyxml2_INCLUDE_DIRS})
    add_subdirectory(extern/libsndfile)
    add_subdirectory(extern/nodeeditor)
endblock()
```

Add `set(BUILD_SHARED_LIBS OFF)` inside the block so external libraries are always static:
```cmake
# Force external libraries to static and disable their tests.
block(SCOPE_FOR VARIABLES)
    set(BUILD_SHARED_LIBS OFF)
    set(BUILD_TESTING OFF)
    set(tinyxml2_BUILD_TESTING OFF)
    add_subdirectory(extern/tinyxml2)
    INCLUDE_DIRECTORIES(${tinyxml2_INCLUDE_DIRS})
    add_subdirectory(extern/libsndfile)
    add_subdirectory(extern/nodeeditor)
endblock()
```

The `block(SCOPE_FOR VARIABLES)` ensures the override does not leak back to the parent scope. Each external static archive will be linked into exactly one BabelWires shared library:
- `tinyxml2` → `BaseLib.so`
- `libsndfile` → `Seq2tapeLib.so`
- `nodeeditor` → `BabelWiresQtUi.so`

Their symbols will not appear in the dynamic symbol tables because later phases set `CXX_VISIBILITY_PRESET hidden` on the consuming targets.

`googletest` is already added separately (line 65: `add_subdirectory(extern/googletest)` inside a `BUILD_TESTING` guard) and is only linked into test executables. It should also be forced static. Wrap it in its own block:
```cmake
if(BUILD_TESTING)
    block(SCOPE_FOR VARIABLES)
        set(BUILD_SHARED_LIBS OFF)
        add_subdirectory(extern/googletest)
    endblock()
endif()
```

#### 3. Verify existing settings remain

- `CMAKE_POSITION_INDEPENDENT_CODE ON` (line 28) — already set, no change needed. Critical: external static libraries must be compiled with `-fPIC` so they can be linked into shared libraries.
- `CMAKE_POLICY_DEFAULT_CMP0077 NEW` (line 22) — already set, no change needed. Ensures submodules respect `BUILD_SHARED_LIBS` (both the global option and the overridden value inside the block).

#### 4. CI

Add a CI matrix entry that builds with `-DBUILD_SHARED_LIBS=ON` alongside the existing static build, so regressions in either mode are caught immediately.

### Acceptance criteria

- `cmake -DBUILD_SHARED_LIBS=OFF ..` produces identical artifacts to today.
- `cmake -DBUILD_SHARED_LIBS=ON ..` configures without error. BabelWires libraries are built as `.so`/`.dll`/`.dylib`. External libraries (`tinyxml2`, `libsndfile`, `nodeeditor`, `googletest`) are always `.a`/`.lib`.
- Default (no `-D` flag) behaves the same as today (static).

### Rollback

Revert the `option()` call back to `set(BUILD_SHARED_LIBS OFF)` and remove the added `set(BUILD_SHARED_LIBS OFF)` from the block scopes.
