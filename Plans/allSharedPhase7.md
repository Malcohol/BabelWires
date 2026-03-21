## Phase 7 — Singleton validation and cleanup (PR 8)

### Goal

Confirm that the global singletons in BaseLib are naturally unique now that `BaseLib.so` is loaded once, and clean up any workarounds.

### Background

Two singletons in BaseLib:
- `IdentifierRegistry` — `static IdentifierRegistry* s_singletonInstance` + `static std::shared_mutex s_mutex` in [BaseLib/Identifiers/identifierRegistry.hpp](BaseLib/Identifiers/identifierRegistry.hpp).
- `DebugLogger` — `static DebugLogger* g_debugLogger` in [BaseLib/Log/debugLogger.hpp](BaseLib/Log/debugLogger.hpp).

With `BaseLib` as a shared library, these static members exist in exactly one place (the `.so`'s data segment). All consumers link against the same `BaseLib.so`, so the singletons are naturally shared.

### Changes required

#### 1. Remove duplication workarounds

Remove any workarounds or defensive checks that were only needed because of potential singleton duplication in a static-linking model.

#### 2. Add singleton uniqueness assertions

Add startup assertions that confirm exactly one copy of each singleton exists (e.g., compare addresses across library boundaries in a diagnostic test).

#### 3. Verify registration pattern

The `ProjectContext`-based registration pattern (explicit `registerLib()` calls) remains correct and unchanged — it already works cleanly across shared library boundaries because it uses dependency-injected context objects rather than global auto-registration.

### Acceptance criteria

- Startup is deterministic; singleton address comparison tests pass.

### Rollback

Re-add defensive checks if singleton uniqueness cannot be guaranteed on a platform.
