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

### Phases

- [Phase 0 — Build-mode switch and CI groundwork (PR 1)](allSharedPhase0.md)
- [Phase 1 — Export macros for BaseLib (PR 2)](allSharedPhase1.md)
- [Phase 2 — Export macros for BabelWiresLib (PR 3)](allSharedPhase2.md)
- [Phase 3 — Export macros for BabelWiresQtUi (PR 4)](allSharedPhase3.md)
- [Phase 4 — Export macros for domain libraries (PR 5)](allSharedPhase4.md)
- [Phase 5 — Test utility libraries and RPATH configuration (PR 6)](allSharedPhase5.md)
- [Phase 6 — SERIALIZABLE macro and inline static hardening (PR 7)](allSharedPhase6.md)
- [Phase 7 — Singleton validation and cleanup (PR 8)](allSharedPhase7.md)
- [Phase 8 — Enable runtime-loadable plugins on top of shared core (PR 9)](allSharedPhase8.md)
- [Phase 9 — Generalize plugin loading and transition completion (PR 10)](allSharedPhase9.md)
- [Phase 10 — Refine export annotations to member level (PR 11)](allSharedPhase10.md)

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
