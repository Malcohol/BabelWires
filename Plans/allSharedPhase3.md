## Phase 3 — Export macros for BabelWiresQtUi (PR 4)

### Goal

Make `BabelWiresQtUi` build as a shared library when `BUILD_SHARED_LIBS=ON`. Handle the `nodeeditor` static library integration correctly.

### Changes required

#### 1. Create export header

Create `BabelWiresQtUi/babelWiresQtUiExport.hpp` with a `BABELWIRESQTUI_API` macro following the same pattern as [Phase 1](allSharedPhase1.md).

#### 2. Update CMakeLists.txt

In [BabelWiresQtUi/CMakeLists.txt](BabelWiresQtUi/CMakeLists.txt):
- Set visibility defaults and `BABELWIRESQTUI_EXPORTS`.
- **Keep** the `NODE_EDITOR_STATIC` compile definition: since `nodeeditor` is always built as a static library (forced in [Phase 0](allSharedPhase0.md)), this define must remain so that nodeeditor's export macros resolve to nothing. The static `nodeeditor` archive gets linked into `BabelWiresQtUi.so` and its symbols are hidden by the visibility preset.

```cmake
if(BUILD_SHARED_LIBS)
    set_target_properties(BabelWiresQtUi PROPERTIES
        CXX_VISIBILITY_PRESET hidden
        VISIBILITY_INLINES_HIDDEN ON)
    target_compile_definitions(BabelWiresQtUi PRIVATE BABELWIRESQTUI_EXPORTS)
endif()
# NODE_EDITOR_STATIC stays — nodeeditor is always static (Phase 0).
```

#### 3. Annotate public API classes

Annotate public API classes at the **class level** in BabelWiresQtUi headers: main window, model bridge, node editor bridge, value editors, value models, context menu, dialogs, utilities.

Member-level refinement is deferred to [Phase 10](allSharedPhase10.md).

### Acceptance criteria

- `libBabelWiresQtUi.so` builds; the application starts normally in shared mode.
- `nodeeditor` symbols are not in `BabelWiresQtUi.so`'s dynamic symbol table (verify with `nm -D`).
- `BUILD_SHARED_LIBS=OFF` still works.

### Rollback

Remove export annotations.
