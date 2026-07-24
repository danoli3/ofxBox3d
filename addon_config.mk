# All variables and this particular order are required for OF addons

meta:
	ADDON_NAME = ofxBox3d
	ADDON_DESCRIPTION = openFrameworks wrapper for Box3D (Erin Catto 3D physics)
	ADDON_AUTHOR = ofxBox3d contributors
	ADDON_TAGS = "physics" "3d" "simulation" "box3d" "rigidbody"
	ADDON_URL = https://github.com/YOUR_USERNAME/ofxBox3d

common:
	# Include path for the public Box3D headers
	ADDON_INCLUDES = libs/box3d/include
	ADDON_INCLUDES += src

	# If you compile Box3D sources directly (instead of linking a prebuilt lib):
	# List them here. You will need to adjust for your platform / SIMD.
	# Example (incomplete – fill from box3d/src after submodule):
	# ADDON_SOURCES = libs/box3d/src/*.c
	# Or better: use CMake / FetchContent in modern OF projects.

linux64:
	# ADDON_LIBS = ... if linking a static lib

osx:
	# ...

vs:
	# ...
