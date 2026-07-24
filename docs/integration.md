# Integrating Box3D with ofxBox3d

Box3D is a C17 library built with CMake. There are two practical ways to consume it from an openFrameworks project.

## 1. Git submodule + CMake (recommended for modern projects)

```bash
cd addons/ofxBox3d/libs
git submodule add https://github.com/erincatto/box3d.git box3d
```

In your application’s `CMakeLists.txt` (OF 0.12+ / ofx CMake):

```cmake
add_subdirectory(${OF_ADDONS_DIR}/ofxBox3d/libs/box3d EXCLUDE_FROM_ALL)
# or FetchContent

target_link_libraries(${APP_NAME} PRIVATE box3d::box3d)
target_include_directories(${APP_NAME} PRIVATE
    ${OF_ADDONS_DIR}/ofxBox3d/src
    ${OF_ADDONS_DIR}/ofxBox3d/libs/box3d/include
)
```

The ofxBox3d sources (`src/*.cpp`) should also be added to your target.

## 2. Compile Box3D sources via addon_config.mk (classic Makefile / PG)

1. Submodule as above.
2. Edit `addon_config.mk` and list every `.c` file under `libs/box3d/src/` in `ADDON_SOURCES`.
3. Add any required defines (`BOX3D_DISABLE_SIMD` if you hit platform issues).
4. The Project Generator / Makefile will compile them together with the ofxBox3d C++ files.

**Note:** Box3D uses a few platform-specific files and optional SIMD. You may need to filter sources per platform (see Box3D’s own CMakeLists for the exact list).

## Scale

Box3D is tuned for real-world meters. The wrapper’s global scale (default 1.0) converts between OF coordinates and Box3D meters:

```cpp
ofxBox3d::setScale(100.f); // 100 OF units = 1 m
```

All position / size arguments to the high-level helpers (`ofxBox3dBox`, `ofxBox3dSphere`, …) are in OF units and are automatically converted.

## Next steps for the addon

- Capsule helper
- Joint wrappers (revolute, distance, weld, …)
- Contact / sensor event system (ofEvent)
- ofMesh / ofVboMesh driven bodies
- Character mover helper
- Better debug draw (using Box3D’s own debug callbacks if exposed)
- Automated source list generation for addon_config.mk
