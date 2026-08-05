#include "ofxBox3dCapsule.h"

bool ofxBox3dCapsule::setup(ofxBox3d& world,
                            const glm::vec3& position,
                            float radius,
                            float height,
                            b3BodyType type,
                            float density,
                            float friction,
                            float restitution) {
    if (!ofxBox3dBody::setup(world, position, type)) return false;

    this->radius = radius;
    this->height = height;
    float halfSegment = std::max(0.f, height * 0.5f - radius) / ofxBox3dUtils::getScale();
    b3Capsule capsule = {};
    capsule.center1 = {0.f, -halfSegment, 0.f};
    capsule.center2 = {0.f, halfSegment, 0.f};
    capsule.radius = radius / ofxBox3dUtils::getScale();
    b3ShapeDef shapeDef = makeShapeDef(density, friction, restitution);
    shapeId = b3CreateCapsuleShape(bodyId, &shapeDef, &capsule);
    if (B3_IS_NULL(shapeId)) {
        destroy();
        return false;
    }
    return true;
}

void ofxBox3dCapsule::draw() const {
    if (!isValid()) return;
    ofPushMatrix();
    ofMultMatrix(getGlobalTransformMatrix());
    ofPushStyle();
    ofSetColor(color);
    if (bFilled) ofFill(); else ofNoFill();
    float cylinderHeight = std::max(0.f, height - 2.f * radius);
    ofDrawCylinder(radius, cylinderHeight);
    ofDrawSphere({0.f, -cylinderHeight * 0.5f, 0.f}, radius);
    ofDrawSphere({0.f, cylinderHeight * 0.5f, 0.f}, radius);
    ofPopStyle();
    ofPopMatrix();
}
