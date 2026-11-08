#include "ofxBox3dCapsule.h"
#include <utility>

ofxBox3dCapsule::ofxBox3dCapsule(ofxBox3dCapsule&& other)
    : ofxBox3dBody(std::move(other)), radius(other.radius), height(other.height),
      shapeId(std::exchange(other.shapeId, b3_nullShapeId)) {}

ofxBox3dCapsule& ofxBox3dCapsule::operator=(ofxBox3dCapsule&& other) {
    if (this != &other) {
        ofxBox3dBody::operator=(std::move(other));
        radius = other.radius;
        height = other.height;
        shapeId = std::exchange(other.shapeId, b3_nullShapeId);
    }
    return *this;
}

bool ofxBox3dCapsule::setup(ofxBox3d& world,
                            const glm::vec3& position,
                            float radius,
                            float height,
                            b3BodyType type,
                            float density,
                            float friction,
                            float restitution) {
    ofxBox3dBodySettings settings;
    settings.type = type;
    settings.density = density;
    settings.friction = friction;
    settings.restitution = restitution;
    return setup(world, position, radius, height, settings);
}

bool ofxBox3dCapsule::setup(ofxBox3d& world, const glm::vec3& position, float radius,
                            float height, const ofxBox3dBodySettings& settings) {
    if (!ofxBox3dBody::setup(world, position, settings)) return false;

    this->radius = radius;
    this->height = height;
    float halfSegment = std::max(0.f, height * 0.5f - radius) / ofxBox3dUtils::getScale();
    b3Capsule capsule = {};
    capsule.center1 = {0.f, -halfSegment, 0.f};
    capsule.center2 = {0.f, halfSegment, 0.f};
    capsule.radius = radius / ofxBox3dUtils::getScale();
    b3ShapeDef shapeDef = makeShapeDef(settings);
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
    ofMultMatrix(getRenderTransformMatrix());
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
