## Phase 10 — Refine export annotations to member level (PR 11)

### Goal

Phases [1](allSharedPhase1.md)–[4](allSharedPhase4.md) annotate at the class level (`class BASELIB_API Foo`) to get a working shared build quickly. This over-exports: every member function, static member, and compiler-generated symbol in the class is visible in the dynamic symbol table. Phase 10 narrows exports to only what consumers actually need.

### Changes required

#### 1. Move annotations from class to member level

For each library, move the `*_API` macro from the class declaration to individual member declarations that form the public API boundary:

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

#### 2. Guidelines for deciding what to annotate

- **Virtual functions**: only the base-class declaration needs export if the vtable is emitted in the library's `.cpp` file. Override declarations in the same library do not need separate annotation.
- **Inline / header-only methods**: do not need export — they are compiled into the consumer's TU.
- **Private and internal-only members**: remove export. This reduces symbol table size and prevents accidental coupling.
- **Template members**: typically header-only and do not need export. Only explicit template instantiations in `.cpp` files need annotation.
- **Compiler-generated special members** (constructors, destructors, copy/move): export only those that consumers call. If a class is only ever handled via `std::unique_ptr` returned from a factory, only the destructor (implicitly called by the deleter) and the factory function need export.

#### 3. Measure improvement

Use `nm -D --defined-only` (Linux) / `dumpbin /EXPORTS` (Windows) before and after to measure the reduction in exported symbols per library.

### Acceptance criteria

- All tests pass; exported symbol counts are reduced; no link failures in consumer libraries or tests.

### Rollback

Revert to class-level annotation (always safe, just over-exports).
