#include "ofxBox3dSphere.h"
#include <utility>

ofxBox3dSphere::ofxBox3dSphere(ofxBox3dSphere&& other)
    : ofxBox3dBody(std::move(other)), radius(other.radius),
      shapeId(std::exchange(other.shapeId, b3_nullShapeId)) {}

ofxBox3dSphere& ofxBox3dSphere::operator=(ofxBox3dSphere&& other) {
    if (this != &other) {
        ofxBox3dBody::operator=(std::move(other));
        radius = other.radius;
        shapeId = std::exchange(other.shapeId, b3_nullShapeId);
    }
    return *this;
}

bool ofxBox3dSphere::setup(ofxBox3d& world,
                           const glm::vec3& position,
                           float radius,
                           b3BodyType type,
                           float density,
                           float friction,
                           float restitution)
{
    ofxBox3dBodySettings settings;
    settings.type = type;
    settings.density = density;
    settings.friction = friction;
    settings.restitution = restitution;
    return setup(world, position, radius, settings);
}

bool ofxBox3dSphere::setup(ofxBox3d& world, const glm::vec3& position, float radius,
                           const ofxBox3dBodySettings& settings) {
    if (!ofxBox3dBody::setup(world, position, settings)) {
        return false;
    }

    this->radius = radius;

    b3Sphere sphere;
    sphere.center = {0.f, 0.f, 0.f};
    sphere.radius = radius / ofxBox3dUtils::getScale();   // convert to meters

    b3ShapeDef shapeDef = makeShapeDef(settings);
    shapeId = b3CreateSphereShape(bodyId, &shapeDef, &sphere);

    if (B3_IS_NULL(shapeId)) {
        ofLogError("ofxBox3dSphere") << "b3CreateSphereShape failed";
        destroy();
        return false;
    }

    return true;
}

void ofxBox3dSphere::draw() const {
    if (!isValid()) return;

    ofPushMatrix();
    ofMultMatrix(getRenderTransformMatrix());
    ofPushStyle();
    ofSetColor(color);
    if (bFilled) ofFill(); else ofNoFill();
    ofDrawSphere(radius);
    ofPopStyle();
    ofPopMatrix();
}
