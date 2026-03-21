## Phase 2 — Export macros for BabelWiresLib (PR 3)

### Goal

Make `BabelWiresLib` build as a shared library when `BUILD_SHARED_LIBS=ON`, following the same pattern established in [Phase 1](allSharedPhase1.md) for BaseLib.

### Changes required

#### 1. Create export header

Create `BabelWiresLib/babelWiresLibExport.hpp` with a `BABELWIRESLIB_API` macro following the same pattern as [Phase 1](allSharedPhase1.md).

#### 2. Update CMakeLists.txt

In [BabelWiresLib/CMakeLists.txt](BabelWiresLib/CMakeLists.txt), set visibility defaults and `BABELWIRESLIB_EXPORTS` private define:

```cmake
if(BUILD_SHARED_LIBS)
    set_target_properties(BabelWiresLib PROPERTIES
        CXX_VISIBILITY_PRESET hidden
        VISIBILITY_INLINES_HIDDEN ON)
    target_compile_definitions(BabelWiresLib PRIVATE BABELWIRESLIB_EXPORTS)
endif()
```

#### 3. Annotate public API classes

Annotate all public API classes at the **class level** (e.g., `class BABELWIRESLIB_API Foo`). Key areas (~130 source files):

- Type system: `TypeSystem`, `Type`, `TypeConstructor`, and all concrete types.
- Project model: `Project`, `Node`, `Modifier`, all command classes.
- Processors: `Processor`, `ProcessorFactory`, `ParallelProcessor`.
- File formats: `SourceFileFormat`, `TargetFileFormat`.
- Value tree: `ValueTreeRoot`, `ValueTreeNode`, `EditTree`.
- Registries: `ProjectContext`, all registry types.
- Serialization support: all `SERIALIZABLE`-annotated classes.
- Path: `Path`, `PathStep`.

Member-level refinement is deferred to [Phase 10](allSharedPhase10.md).

### Acceptance criteria

- `libBabelWiresLib.so` builds and links; all BabelWiresLib tests pass.
- `BUILD_SHARED_LIBS=OFF` still works.

### Rollback

Remove export header and annotations.
