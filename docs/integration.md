# Integrating Box3D with ofxBox3d

Box3D is a C17 library built with CMake. There are two practical ways to consume it from an openFrameworks project.

## 1. Git submodule + CMake

```bash
cd addons/ofxBox3d
git submodule update --init --recursive
```

In your application's `CMakeLists.txt`, after configuring its openFrameworks target:

```cmake
add_subdirectory(${OF_ADDONS_DIR}/ofxBox3d ${CMAKE_BINARY_DIR}/ofxBox3d)
target_link_libraries(${APP_NAME} PRIVATE ofxBox3d::ofxBox3d)
```

The interface target adds the wrapper sources to your app and links `box3d::box3d`.
Your app must already have the openFrameworks includes, libraries, and platform
settings. CMake 3.22 or later is required. A standalone build from the addon root
builds the Box3D library without requiring openFrameworks:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
```

## 2. Compile Box3D sources via addon_config.mk (classic Makefile / PG)

Initialize the submodule as above, add `ofxBox3d` to `addons.make`, and generate
your project. The addon configuration excludes upstream tests, samples,
benchmarks, shared sample code, documentation, and external sample dependencies.
The Makefile / Project Generator compiles the Box3D C sources and wrapper C++
sources together. Regenerate existing Xcode / Visual Studio projects after an
update to pick up renamed and added upstream sources.

## SIMD

SIMD is **on by default** in CMake, addon source builds, and the Apothecary
formula. Box3D selects NEON on ARM64 and SSE2 on x86. Supported x86 builds also
compile a separate AVX2 path and select it at runtime. Unsupported architectures
use upstream's scalar fallback.

The native CMake switch is `BOX3D_DISABLE_SIMD`, defaulting to `OFF`. To build
without SIMD:

```bash
cmake -S . -B build-scalar -DCMAKE_BUILD_TYPE=Release -DBOX3D_DISABLE_SIMD=ON
cmake --build build-scalar --parallel
```

For Makefile / Project Generator projects, uncomment this line in the `common:`
section of `addon_config.mk`, regenerate, and rebuild:

```make
ADDON_DEFINES += BOX3D_DISABLE_SIMD
```

Leave that macro **undefined** for SIMD builds. Defining it as `0` still disables
SIMD because upstream tests whether it is defined. Do not define upstream's
internal `B3_SIMD_*` macros yourself.

Visual Studio Project Generator builds define `BOX3D_DISABLE_AVX2`, keeping SSE2
enabled, because PG cannot apply `/arch:AVX2` only to `wide8.c`. CMake applies
upstream's per-file flags and retains runtime AVX2 dispatch. Emscripten addon
builds enable `-msimd128 -msse2`; their output requires WebAssembly SIMD support.

## Scale

Box3D is tuned for real-world meters. The wrapper’s global scale (default 1.0) converts between OF coordinates and Box3D meters:

```cpp
ofxBox3dUtils::setScale(100.f); // 100 OF units = 1 m
```

All position / size arguments to the high-level helpers (`ofxBox3dBox`, `ofxBox3dSphere`, …) are in OF units and are automatically converted.

## Wrapper API and precision

The [README](../README.md#factories-and-settings) describes value-returning factories,
shared creation settings, and the existing shared-pointer workflow. Value-owned
bodies simulate without registration; draw them individually, and call
`syncFromPhysics()` when another object needs their inherited `ofNode` transform.

`BOX3D_DOUBLE_PRECISION=ON` enables double-precision world positions in CMake.
For source-based addon projects, enable the matching define in `addon_config.mk`.
Rebuild Box3D and your app together. Use `setPositionDouble()` and
`getPositionDouble()` to preserve precision; `ofNode` and OF rendering use floats.

The optional `OFXBOX3D_BUILD_TESTS` CMake setting builds `ofxBox3d_tests`. Define
an openFrameworks target first and set `OFXBOX3D_OPENFRAMEWORKS_TARGET` to its name
before adding this addon. The regression suite runs without a window and covers
lifetime, move ownership, factories/settings, and both position precisions.

## Next steps for the addon

- Joint wrappers (revolute, distance, weld, …)
- ofMesh / ofVboMesh driven bodies
- Character mover helper
- Better debug draw (using Box3D’s own debug callbacks if exposed)
- Automated source list generation for addon_config.mk
