#pragma once

#include "ofMain.h"
#include <box3d/box3d.h>   // assumes include path points to box3d/include

namespace ofxBox3d {

// Global scale: OF units per meter (default 1.0 → 1 OF unit = 1 meter)
// Use larger values (e.g. 100) if you prefer working in centimetres / pixels-like units.
inline float& scale() {
    static float s = 1.0f;
    return s;
}

inline void setScale(float s) { scale() = s; }
inline float getScale() { return scale(); }

// -----------------------------------------------------------------------------
// Vector conversion
// -----------------------------------------------------------------------------
inline glm::vec3 toOf(const b3Vec3& v) {
    float s = getScale();
    return { v.x * s, v.y * s, v.z * s };
}

inline b3Vec3 toB3(const glm::vec3& v) {
    float s = getScale();
    return { v.x / s, v.y / s, v.z / s };
}

inline b3Vec3 toB3(float x, float y, float z) {
    return toB3({x, y, z});
}

// -----------------------------------------------------------------------------
// Quaternion conversion  (Box3D: vector part + scalar; OF/glm: x,y,z,w)
// -----------------------------------------------------------------------------
inline ofQuaternion toOf(const b3Quat& q) {
    // ofQuaternion(x, y, z, w)
    return ofQuaternion(q.v.x, q.v.y, q.v.z, q.s);
}

inline glm::quat toGlm(const b3Quat& q) {
    return glm::quat(q.s, q.v.x, q.v.y, q.v.z); // glm is w,x,y,z
}

inline b3Quat toB3(const ofQuaternion& q) {
    b3Quat r;
    r.v = { q.x(), q.y(), q.z() };
    r.s = q.w();
    return r;
}

inline b3Quat toB3(const glm::quat& q) {
    b3Quat r;
    r.v = { q.x, q.y, q.z };
    r.s = q.w;
    return r;
}

// -----------------------------------------------------------------------------
// Transform helpers
// -----------------------------------------------------------------------------
inline void applyBodyTransform(ofNode& node, b3BodyId bodyId) {
    if (B3_IS_NULL(bodyId)) return;
    b3Vec3 pos = b3Body_GetPosition(bodyId);
    b3Quat rot = b3Body_GetRotation(bodyId);
    node.setPosition(toOf(pos));
    node.setOrientation(toOf(rot));
}

inline glm::mat4 getBodyMatrix(b3BodyId bodyId) {
    if (B3_IS_NULL(bodyId)) return glm::mat4(1.f);
    b3Vec3 p = b3Body_GetPosition(bodyId);
    b3Quat q = b3Body_GetRotation(bodyId);
    glm::mat4 T = glm::translate(glm::mat4(1.f), toOf(p));
    glm::mat4 R = glm::mat4_cast(toGlm(q));
    return T * R;
}

} // namespace ofxBox3d
