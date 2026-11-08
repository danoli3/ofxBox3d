cmake_minimum_required(VERSION 3.22)

project(ofxBox3d LANGUAGES C CXX)

# Use Box3D's native option: SIMD is on unless explicitly disabled.
option(BOX3D_DISABLE_SIMD "Build Box3D without SIMD" OFF)
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/libs/box3d/CMakeLists.txt")
    message(FATAL_ERROR "Initialize Box3D with: git submodule update --init --recursive")
endif()

add_subdirectory(libs/box3d)

# Compile the wrapper in the consuming OF target so it inherits that target's
# openFrameworks headers, platform definitions, and compiler settings.
add_library(ofxBox3d INTERFACE)
add_library(ofxBox3d::ofxBox3d ALIAS ofxBox3d)
target_sources(ofxBox3d INTERFACE
    "${CMAKE_CURRENT_SOURCE_DIR}/src/ofxBox3d.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/ofxBox3dBody.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/ofxBox3dBox.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/ofxBox3dCapsule.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/ofxBox3dSphere.cpp"
)
target_include_directories(ofxBox3d INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/src")
target_compile_features(ofxBox3d INTERFACE cxx_std_17)
target_link_libraries(ofxBox3d INTERFACE box3d::box3d)

option(OFXBOX3D_BUILD_TESTS "Build wrapper regression tests (requires an OF target)" OFF)
set(OFXBOX3D_OPENFRAMEWORKS_TARGET "" CACHE STRING "Existing openFrameworks CMake target for wrapper tests")
if(OFXBOX3D_BUILD_TESTS)
    if(NOT TARGET "${OFXBOX3D_OPENFRAMEWORKS_TARGET}")
        message(FATAL_ERROR "Set OFXBOX3D_OPENFRAMEWORKS_TARGET to an existing openFrameworks target")
    endif()
    enable_testing()
    add_executable(ofxBox3d_tests tests/wrapper_tests.cpp)
    target_link_libraries(ofxBox3d_tests PRIVATE ofxBox3d::ofxBox3d "${OFXBOX3D_OPENFRAMEWORKS_TARGET}")
    add_test(NAME ofxBox3d_wrapper COMMAND ofxBox3d_tests)
endif()
