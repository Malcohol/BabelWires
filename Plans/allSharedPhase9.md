## Phase 9 — Generalize plugin loading and transition completion (PR 10)

### Goal

Roll out runtime plugin loading to all domain plugins, make shared libraries the default build mode, and clean up.

### Changes required

#### 1. Convert remaining plugins

Convert remaining domain plugins (`TestPluginLib`, and any future plugins) to runtime-loaded modules.

#### 2. Categorize domain libraries

Separate domain libraries into two categories:
- **Core domain libraries** (`musicLib`, `musicLibUi`) — remain linked at build time since they provide foundational types that plugins depend on.
- **Plugins** (`SmfLib`, `TestPluginLib`, future additions) — runtime-loaded, depending on the core domain libraries' shared objects.

#### 3. Default to shared

Set `BUILD_SHARED_LIBS=ON` as the default in [CMakeLists.txt](CMakeLists.txt). Keep `OFF` as a supported fallback for one release cycle.

#### 4. Cleanup

Remove obsolete static-only shims and compatibility code.

#### 5. Documentation

- Update [README.md](README.md) with build instructions for shared mode, plugin development guide, and platform-specific notes (RPATH, DLL paths).
- Update [TODO.md](TODO.md) to reflect completed migration.

### Acceptance criteria

- Shared libraries are the default; all tests pass on all platforms; plugin loading works; static fallback still compiles.

### Rollback

Flip default back to `OFF`.
