#pragma once

#include "ofxBox3dBody.h"

class ofxBox3dSphere : public ofxBox3dBody {
public:
    ofxBox3dSphere() = default;
    ofxBox3dSphere(ofxBox3dSphere&& other);
    ofxBox3dSphere& operator=(ofxBox3dSphere&& other);

    bool setup(ofxBox3d& world, const glm::vec3& position, float radius,
               const ofxBox3dBodySettings& settings);

    bool setup(ofxBox3d& world,
               const glm::vec3& position,
               float radius,
               b3BodyType type = b3_dynamicBody,
               float density = 1.f,
               float friction = 0.3f,
               float restitution = 0.f);

    float getRadius() const { return radius; }

    void draw() const override;

protected:
    float radius = 0.5f;
    b3ShapeId shapeId = b3_nullShapeId;
};
