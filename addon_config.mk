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

	ADDON_SOURCES_EXCLUDE = libs/box3d/benchmark/%
	ADDON_SOURCES_EXCLUDE += libs/box3d/samples/%
	ADDON_SOURCES_EXCLUDE += libs/box3d/shared/%
	ADDON_SOURCES_EXCLUDE += libs/box3d/test/%

linux64:
	# Source build is the default. Prebuilt archives are optional release assets.

osx:
	# Source build is the default. Prebuilt archives are optional release assets.

vs:
	# Source build is the default. Prebuilt archives are optional release assets.
