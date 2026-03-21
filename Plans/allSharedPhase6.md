## Phase 6 — SERIALIZABLE macro and inline static hardening (PR 7)

### Goal

Verify that the `SERIALIZABLE` macro's inline static members and serialization tag mechanism work correctly across shared library boundaries.

### Background

The `SERIALIZABLE` macro (in [BaseLib/Serialization/serializable.hpp](BaseLib/Serialization/serializable.hpp)) generates per-class:
- `static inline const DeserializationRegistryInterface::Entry s_registryEntry{...}` — contains factory, type name, version.
- `Detail::SerializableType<T>::s_serializationTag` — a `const void*` whose address serves as a unique type identifier.

With C++17 inline variable rules and shared linkage, these should have a single instance per type across the process. However, this needs verification.

### Changes required

#### 1. Audit `inline static` variables

- Verify that `inline static` variables in header-included templates have a single instance per type across the process on all target platforms (GCC, Clang, MSVC).
- If any library is accidentally included via both static and shared paths, the ODR guarantee is violated. Add a CMake check or link-time assertion that prevents mixing.

#### 2. Audit `s_serializationTag`

Same inline-variable concerns as above for `Detail::SerializableType<T>::s_serializationTag`.

#### 3. Fix edge cases

If any edge cases are found, move the static storage into a `.cpp` file with explicit instantiation, or use a registry-based approach instead of inline statics.

### Acceptance criteria

- Serialization round-trip tests pass.
- No duplicate `type_info` or `s_registryEntry` instances detected (verify with `nm -D` / `dumpbin`).

### Rollback

Keep explicit registration as-is; inline statics are harmless in static-linking mode.
