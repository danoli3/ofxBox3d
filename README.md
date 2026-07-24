# ofxBox3d

openFrameworks wrapper for [Box3D](https://github.com/erincatto/box3d) — Erin Catto's open-source 3D rigid-body physics engine (the 3D successor to Box2D).

**Status:** Work in progress / alpha (matching Box3D 0.1.x)

## Features (planned / current)

- Thin, modern C++ wrapper around the C17 Box3D API
- `ofxBox3d` world manager with fixed-timestep stepping
- Body helpers: Box, Sphere, Capsule (+ base class for custom shapes)
- Easy conversion between `glm::vec3` / `ofQuaternion` and Box3D types
- Optional scale factor (OF units ↔ meters)
- Debug drawing
- Contact / sensor event hooks (via ofEvents)
- ofNode-friendly transforms for easy rendering

## Requirements

- openFrameworks 0.12+ (tested against master)
- C++17 compiler
- CMake ≥ 3.16 (for building Box3D itself)
- Box3D source (git submodule recommended)

## Installation

```bash
cd openFrameworks/addons
git clone https://github.com/YOUR_USERNAME/ofxBox3d.git
cd ofxBox3d
git submodule update --init --recursive   # or manually clone box3d into libs/box3d
```

### Integrating Box3D

Recommended: git submodule

```bash
cd libs
git submodule add https://github.com/erincatto/box3d.git box3d
```

Then either:

1. **Preferred (CMake projects):** Use `FetchContent` or `add_subdirectory(libs/box3d)` in your app’s CMakeLists and link `box3d::box3d`.
2. **Classic OF Makefile / Project Generator:** The addon’s `addon_config.mk` will list the Box3D sources so they are compiled into your project. (You may need to adjust for your platform / SIMD flags.)

See `docs/integration.md` for details.

## Quick Start

```cpp
#include "ofxBox3d.h"

ofxBox3d physics;

void ofApp::setup() {
    physics.setup();                       // default gravity (0, -10, 0)
    // physics.setGravity({0, -9.81f, 0});
    // physics.setScale(100.f);            // 100 OF units = 1 meter (optional)

    auto ground = make_shared<ofxBox3dBox>();
    ground->setup(physics, {0, -1, 0}, {20, 1, 20});  // pos, half-extents
    ground->setStatic();

    auto box = make_shared<ofxBox3dBox>();
    box->setup(physics, {0, 5, 0}, {0.5f, 0.5f, 0.5f});
    box->setDensity(1.f);
    box->setFriction(0.3f);
}

void ofApp::update() {
    physics.update();   // steps at 60 Hz with 4 sub-steps by default
}

void ofApp::draw() {
    ofEnableDepthTest();
    cam.begin();
    physics.draw();     // debug draw all registered bodies
    // or draw individual bodies / your own meshes driven by body transforms
    cam.end();
}
```

## License

MIT (same as Box3D and typical OF addons)

Box3D itself is MIT — © Erin Catto.

## Credits

- Box3D by [Erin Catto](https://github.com/erincatto)
- Inspired by [ofxBox2d](https://github.com/vanderlin/ofxBox2d) by Todd Vanderlin
