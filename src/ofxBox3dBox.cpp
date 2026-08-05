#include "ofxBox3dBox.h"

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
    if (!ofxBox3dBody::setup(world, position, rotation, type)) {
        return false;
    }

    this->halfExtents = halfExtents;

    // Convert half-extents to Box3D (meters)
    b3Vec3 he = ofxBox3dUtils::toB3(halfExtents);
    b3BoxHull boxHull = b3MakeBoxHull(he.x, he.y, he.z);

    b3ShapeDef shapeDef = makeShapeDef(density, friction, restitution);
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
    ofMultMatrix(getGlobalTransformMatrix());
    ofPushStyle();
    ofSetColor(color);
    if (bFilled) ofFill(); else ofNoFill();
    ofDrawBox(halfExtents.x * 2.f, halfExtents.y * 2.f, halfExtents.z * 2.f);
    ofPopStyle();
    ofPopMatrix();
}
