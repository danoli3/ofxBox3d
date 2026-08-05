# Box3D binary tooling

The addon pins Box3D through `libs/box3d`. CI runs that revision's native unit
tests on macOS, Linux and Windows. The separate binary workflow builds static
libraries with openFrameworks Apothecary and can publish them under a `libs-v*`
GitHub release.

```bash
./scripts/platforms.sh
./scripts/build_libs.sh host
./scripts/package_libs.sh host
```

Set `APOTHECARY_DIR` to an existing Apothecary checkout, or the build script
clones the `bleeding` branch into `scripts/.cache`. Release archives contain
`box3d/include`, `box3d/lib/<openFrameworks platform>`, the license, and build
metadata. Trigger **Build and release Box3D binaries** manually with
`upload_release=true`, or push a `libs-v*` tag.
