## Phase 4 — Export macros for domain libraries (PR 5)

### Goal

Make all domain libraries build as shared libraries when `BUILD_SHARED_LIBS=ON`, following the pattern established in Phases [1](allSharedPhase1.md)–[3](allSharedPhase3.md).

### Changes required

#### 1. Create export headers and annotate public symbols

For each domain library, create an export header and annotate public API classes at the **class level**:

| Library | Export header | Macro | CMake define |
|---------|-------------|-------|-------------|
| `musicLib` | `Domains/Music/MusicLib/musicLibExport.hpp` | `MUSICLIB_API` | `MUSICLIB_EXPORTS` |
| `musicLibUi` | `Domains/Music/MusicLibUi/musicLibUiExport.hpp` | `MUSICLIBUI_API` | `MUSICLIBUI_EXPORTS` |
| `SmfLib` | `Domains/Music/Plugins/Smf/Smf/smfLibExport.hpp` | `SMFLIB_API` | `SMFLIB_EXPORTS` |
| `TestPluginLib` | `Domains/Music/Plugins/TestPlugin/testPluginLibExport.hpp` | `TESTPLUGINLIB_API` | `TESTPLUGINLIB_EXPORTS` |
| `Seq2tapeLib` | `Domains/Music/Seq2tapeLib/seq2tapeLibExport.hpp` | `SEQ2TAPELIB_API` | `SEQ2TAPELIB_EXPORTS` |
| `libTestDomain` | `Domains/TestDomain/testDomainExport.hpp` | `TESTDOMAIN_API` | `TESTDOMAIN_EXPORTS` |

Each export header follows the same pattern as [Phase 1](allSharedPhase1.md).

#### 2. Update CMakeLists.txt files

For each library, add visibility defaults and a private `*_EXPORTS` define in its `CMakeLists.txt`:

```cmake
if(BUILD_SHARED_LIBS)
    set_target_properties(<target> PROPERTIES
        CXX_VISIBILITY_PRESET hidden
        VISIBILITY_INLINES_HIDDEN ON)
    target_compile_definitions(<target> PRIVATE <TARGET>_EXPORTS)
endif()
```

Member-level refinement is deferred to [Phase 10](allSharedPhase10.md).

### Acceptance criteria

- All domain `.so` files build; domain tests pass.
- `BUILD_SHARED_LIBS=OFF` still works.

### Rollback

Remove export headers and annotations per library.
