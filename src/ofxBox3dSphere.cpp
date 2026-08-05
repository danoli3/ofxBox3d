#include "ofxBox3dSphere.h"

bool ofxBox3dSphere::setup(ofxBox3d& world,
                           const glm::vec3& position,
                           float radius,
                           b3BodyType type,
                           float density,
                           float friction,
                           float restitution)
{
    if (!ofxBox3dBody::setup(world, position, ofQuaternion(), type)) {
        return false;
    }

    this->radius = radius;

    b3Sphere sphere;
    sphere.center = {0.f, 0.f, 0.f};
    sphere.radius = radius / ofxBox3dUtils::getScale();   // convert to meters

    b3ShapeDef shapeDef = makeShapeDef(density, friction, restitution);
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
    ofMultMatrix(getGlobalTransformMatrix());
    ofPushStyle();
    ofSetColor(color);
    if (bFilled) ofFill(); else ofNoFill();
    ofDrawSphere(radius);
    ofPopStyle();
    ofPopMatrix();
}
