#pragma once

#include "ofxBox3dBody.h"

/**
 * Axis-aligned (in local space) box body using Box3D hull shapes.
 *
 * halfExtents are in OF units (will be converted by the global scale).
 */
class ofxBox3dBox : public ofxBox3dBody {
public:
    ofxBox3dBox() = default;

    /// Create a dynamic or static box
    bool setup(ofxBox3d& world,
               const glm::vec3& position,
               const glm::vec3& halfExtents,
               b3BodyType type = b3_dynamicBody,
               float density = 1.f,
               float friction = 0.3f,
               float restitution = 0.f);

    bool setup(ofxBox3d& world,
               const glm::vec3& position,
               const ofQuaternion& rotation,
               const glm::vec3& halfExtents,
               b3BodyType type = b3_dynamicBody,
               float density = 1.f,
               float friction = 0.3f,
               float restitution = 0.f);

    glm::vec3 getHalfExtents() const { return halfExtents; }

    void draw() const override;

protected:
    glm::vec3 halfExtents{0.5f, 0.5f, 0.5f};
    b3ShapeId shapeId = b3_nullShapeId;
};
