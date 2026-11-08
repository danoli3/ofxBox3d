#include "ofxBox3dBox.h"
#include <utility>

ofxBox3dBox::ofxBox3dBox(ofxBox3dBox&& other)
    : ofxBox3dBody(std::move(other)), halfExtents(other.halfExtents),
      shapeId(std::exchange(other.shapeId, b3_nullShapeId)) {}

ofxBox3dBox& ofxBox3dBox::operator=(ofxBox3dBox&& other) {
    if (this != &other) {
        ofxBox3dBody::operator=(std::move(other));
        halfExtents = other.halfExtents;
        shapeId = std::exchange(other.shapeId, b3_nullShapeId);
    }
    return *this;
}

bool ofxBox3dBox::setup(ofxBox3d& world,
                        const glm::vec3& position,
                        const glm::vec3& halfExtents,
                        b3BodyType type,
                        float density,
                        float friction,
                        float restitution)
{
    return setup(world, position, ofQuaternion(), halfExtents, type, density, friction, restitution);
}

bool ofxBox3dBox::setup(ofxBox3d& world,
                        const glm::vec3& position,
                        const ofQuaternion& rotation,
                        const glm::vec3& halfExtents,
                        b3BodyType type,
                        float density,
                        float friction,
                        float restitution)
{
    ofxBox3dBodySettings settings;
    settings.type = type;
    settings.rotation = rotation;
    settings.density = density;
    settings.friction = friction;
    settings.restitution = restitution;
    return setup(world, position, halfExtents, settings);
}

bool ofxBox3dBox::setup(ofxBox3d& world, const glm::vec3& position,
                        const glm::vec3& halfExtents, const ofxBox3dBodySettings& settings) {
    if (!ofxBox3dBody::setup(world, position, settings)) {
        return false;
    }

    this->halfExtents = halfExtents;

    // Convert half-extents to Box3D (meters)
    b3Vec3 he = ofxBox3dUtils::toB3(halfExtents);
    b3BoxHull boxHull = b3MakeBoxHull(he.x, he.y, he.z);

    b3ShapeDef shapeDef = makeShapeDef(settings);
    shapeId = b3CreateHullShape(bodyId, &shapeDef, &boxHull.base);

    if (B3_IS_NULL(shapeId)) {
        ofLogError("ofxBox3dBox") << "b3CreateHullShape failed";
        destroy();
        return false;
    }

    return true;
}

void ofxBox3dBox::draw() const {
    if (!isValid()) return;

    ofPushMatrix();
    ofMultMatrix(getRenderTransformMatrix());
    ofPushStyle();
    ofSetColor(color);
    if (bFilled) ofFill(); else ofNoFill();
    ofDrawBox(halfExtents.x * 2.f, halfExtents.y * 2.f, halfExtents.z * 2.f);
    ofPopStyle();
    ofPopMatrix();
}
