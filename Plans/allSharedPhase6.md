## Phase 6 — Test utility libraries and RPATH configuration (PR 7)

### Goal

Ensure all test executables can find shared libraries at runtime and decide whether test utility libraries should be shared or static.

### Changes required

#### 1. Test utility libraries

Test utility libraries (`testUtils`, `libTestUtils`, `seqTestUtils`) only exist for testing and need not be shared. Mark them `STATIC` explicitly so they are not affected by the global `BUILD_SHARED_LIBS` setting:

```cmake
ADD_LIBRARY( testUtils STATIC ${TEST_UTILS_SRCS} )
```

#### 2. RPATH configuration

Configure RPATH on all executable targets so shared libraries are found at both build time and install time. Add to the top-level [CMakeLists.txt](CMakeLists.txt):

```cmake
if(BUILD_SHARED_LIBS)
	set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
	set(CMAKE_BUILD_RPATH_USE_ORIGIN TRUE)
	# Optionally per-target:
	# set_target_properties(babelWires PROPERTIES
	#     BUILD_RPATH "$ORIGIN/../lib"
	#     INSTALL_RPATH "$ORIGIN/../lib")
endif()
```

#### 3. Windows DLL co-location

On Windows, ensure all `.dll` files end up in the same directory as executables:

```cmake
if(BUILD_SHARED_LIBS AND WIN32)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
endif()
```

Or configure `PATH` in CTest environment.

#### 4. Test discovery

Ensure `gtest_discover_tests()` finds shared libraries at test run time by setting `ENVIRONMENT` properties or output directories.

### Acceptance criteria

- `ctest` passes all tests in `BUILD_SHARED_LIBS=ON` mode. No "library not found" runtime failures.
- `BUILD_SHARED_LIBS=OFF` still works.

### Rollback

Revert RPATH/output-directory changes; static mode unaffected.
