# All variables and this particular order are required for OF addons

meta:
	ADDON_NAME = ofxBox3d
	ADDON_DESCRIPTION = openFrameworks wrapper for Box3D (Erin Catto 3D physics)
	ADDON_AUTHOR = ofxBox3d contributors
	ADDON_TAGS = "physics" "3d" "simulation" "box3d" "rigidbody"
	ADDON_URL = https://github.com/danoli3/ofxBox3d

common:
	# Include path for the public Box3D headers
	ADDON_INCLUDES = libs/box3d/include
	ADDON_INCLUDES += src

	# Box3D enables NEON (ARM64) or SSE2/AVX2 (x86) automatically.
	# Scalar opt-out: uncomment the following define, then regenerate/rebuild.
	# Do not set BOX3D_DISABLE_SIMD=0: upstream checks whether it is defined.
	# ADDON_DEFINES += BOX3D_DISABLE_SIMD
	# Optional ABI change: rebuild all sources and the app together.
	# ADDON_DEFINES += BOX3D_DOUBLE_PRECISION

	ADDON_SOURCES_EXCLUDE = libs/box3d/benchmark/%
	ADDON_SOURCES_EXCLUDE += libs/box3d/extern/%
	ADDON_SOURCES_EXCLUDE += libs/box3d/docs/%
	ADDON_SOURCES_EXCLUDE += libs/box3d/samples/%
	ADDON_SOURCES_EXCLUDE += libs/box3d/shared/%
	ADDON_SOURCES_EXCLUDE += libs/box3d/test/%

linux64:
	# Source build is the default. Prebuilt archives are optional release assets.
	ADDON_CFLAGS += -ffp-contract=off

osx:
	# Source build is the default. Prebuilt archives are optional release assets.
	ADDON_CFLAGS += -ffp-contract=off

vs:
	# Source build is the default. Prebuilt archives are optional release assets.
	# PG cannot apply /arch:AVX2 to wide8.c alone. Keep portable SSE2 enabled.
	# CMake builds retain upstream's AVX2 runtime dispatch and per-file flags.
	ADDON_DEFINES += BOX3D_DISABLE_AVX2

emscripten:
	ADDON_CFLAGS += -ffp-contract=off -msimd128 -msse2
	ADDON_LDFLAGS += -msimd128
