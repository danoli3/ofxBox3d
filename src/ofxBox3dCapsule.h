#pragma once

#include "ofxBox3dBody.h"

class ofxBox3dCapsule : public ofxBox3dBody {
public:
    ofxBox3dCapsule() = default;
    ofxBox3dCapsule(ofxBox3dCapsule&& other);
    ofxBox3dCapsule& operator=(ofxBox3dCapsule&& other);

    bool setup(ofxBox3d& world, const glm::vec3& position, float radius, float height,
               const ofxBox3dBodySettings& settings);
    bool setup(ofxBox3d& world,
               const glm::vec3& position,
               float radius,
               float height,
               b3BodyType type = b3_dynamicBody,
               float density = 1.f,
               float friction = 0.3f,
               float restitution = 0.f);

    float getRadius() const { return radius; }
    float getHeight() const { return height; }
    void draw() const override;

protected:
    float radius = 0.5f;
    float height = 1.f;
    b3ShapeId shapeId = b3_nullShapeId;
};
