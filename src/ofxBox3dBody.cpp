#include "ofxBox3dBody.h"

ofxBox3dBody::~ofxBox3dBody() {
    destroy();
}

bool ofxBox3dBody::setup(ofxBox3d& world, const glm::vec3& position, b3BodyType type) {
    return setup(world, position, ofQuaternion(), type);
}

bool ofxBox3dBody::setup(ofxBox3d& world,
                         const glm::vec3& position,
                         const ofQuaternion& rotation,
                         b3BodyType type)
{
    if (!world.isSetup()) {
        ofLogError("ofxBox3dBody") << "world not set up";
        return false;
    }
    destroy(); // safety

    worldPtr = &world;

    b3BodyDef def = b3DefaultBodyDef();
    def.type = type;
    def.position = ofxBox3d::toB3(position);
    def.rotation = ofxBox3d::toB3(rotation);

    bodyId = b3CreateBody(world.getWorldId(), &def);
    if (B3_IS_NULL(bodyId)) {
        ofLogError("ofxBox3dBody") << "b3CreateBody failed";
        return false;
    }

    // initialise ofNode
    ofNode::setPosition(position);
    ofNode::setOrientation(rotation);

    return true;
}

void ofxBox3dBody::destroy() {
    if (B3_IS_NON_NULL(bodyId) && worldPtr && worldPtr->isSetup()) {
        // Box3D destroys shapes when the body is destroyed
        b3DestroyBody(bodyId);
    }
    bodyId = b3_nullBodyId;
    worldPtr = nullptr;
}

void ofxBox3dBody::syncFromPhysics() {
    if (!isValid()) return;
    b3Vec3 p = b3Body_GetPosition(bodyId);
    b3Quat q = b3Body_GetRotation(bodyId);
    ofNode::setPosition(ofxBox3d::toOf(p));
    ofNode::setOrientation(ofxBox3d::toOf(q));
}

void ofxBox3dBody::syncToPhysics() {
    if (!isValid()) return;
    b3Body_SetTransform(bodyId,
                        ofxBox3d::toB3(ofNode::getPosition()),
                        ofxBox3d::toB3(ofNode::getOrientationQuat()));
}

glm::vec3 ofxBox3dBody::getPosition() const {
    if (!isValid()) return ofNode::getPosition();
    return ofxBox3d::toOf(b3Body_GetPosition(bodyId));
}

ofQuaternion ofxBox3dBody::getOrientationQuat() const {
    if (!isValid()) return ofNode::getOrientationQuat();
    return ofxBox3d::toOf(b3Body_GetRotation(bodyId));
}

void ofxBox3dBody::setPosition(const glm::vec3& p) {
    ofNode::setPosition(p);
    if (isValid()) {
        b3Body_SetTransform(bodyId, ofxBox3d::toB3(p), b3Body_GetRotation(bodyId));
    }
}

void ofxBox3dBody::setOrientation(const ofQuaternion& q) {
    ofNode::setOrientation(q);
    if (isValid()) {
        b3Body_SetTransform(bodyId, b3Body_GetPosition(bodyId), ofxBox3d::toB3(q));
    }
}

void ofxBox3dBody::setTransform(const glm::vec3& p, const ofQuaternion& q) {
    ofNode::setPosition(p);
    ofNode::setOrientation(q);
    if (isValid()) {
        b3Body_SetTransform(bodyId, ofxBox3d::toB3(p), ofxBox3d::toB3(q));
    }
}

void ofxBox3dBody::setLinearVelocity(const glm::vec3& v) {
    if (isValid()) b3Body_SetLinearVelocity(bodyId, ofxBox3d::toB3(v));
}

glm::vec3 ofxBox3dBody::getLinearVelocity() const {
    if (!isValid()) return {0,0,0};
    return ofxBox3d::toOf(b3Body_GetLinearVelocity(bodyId));
}

void ofxBox3dBody::setAngularVelocity(const glm::vec3& w) {
    if (isValid()) b3Body_SetAngularVelocity(bodyId, ofxBox3d::toB3(w));
}

glm::vec3 ofxBox3dBody::getAngularVelocity() const {
    if (!isValid()) return {0,0,0};
    return ofxBox3d::toOf(b3Body_GetAngularVelocity(bodyId));
}

void ofxBox3dBody::applyForce(const glm::vec3& force, const glm::vec3& point) {
    if (isValid()) b3Body_ApplyForce(bodyId, ofxBox3d::toB3(force), ofxBox3d::toB3(point), true);
}

void ofxBox3dBody::applyForceToCenter(const glm::vec3& force) {
    if (isValid()) b3Body_ApplyForceToCenter(bodyId, ofxBox3d::toB3(force), true);
}

void ofxBox3dBody::applyTorque(const glm::vec3& torque) {
    if (isValid()) b3Body_ApplyTorque(bodyId, ofxBox3d::toB3(torque), true);
}

void ofxBox3dBody::applyLinearImpulse(const glm::vec3& impulse, const glm::vec3& point) {
    if (isValid()) b3Body_ApplyLinearImpulse(bodyId, ofxBox3d::toB3(impulse), ofxBox3d::toB3(point), true);
}

void ofxBox3dBody::applyAngularImpulse(const glm::vec3& impulse) {
    if (isValid()) b3Body_ApplyAngularImpulse(bodyId, ofxBox3d::toB3(impulse), true);
}

void ofxBox3dBody::setType(b3BodyType type) {
    if (isValid()) b3Body_SetType(bodyId, type);
}

b3BodyType ofxBox3dBody::getType() const {
    if (!isValid()) return b3_staticBody;
    return b3Body_GetType(bodyId);
}

void ofxBox3dBody::setStatic()    { setType(b3_staticBody); }
void ofxBox3dBody::setDynamic()   { setType(b3_dynamicBody); }
void ofxBox3dBody::setKinematic() { setType(b3_kinematicBody); }

void ofxBox3dBody::setDensity(float density) {
    if (!isValid()) return;
    // Apply to every shape on the body
    // (Box3D API may require iterating shapes; for now we log a reminder)
    // TODO: implement full shape iteration once we have shape storage
    ofLogVerbose("ofxBox3dBody") << "setDensity – call after shapes are created; "
                                    "prefer setting density on shape creation";
}

void ofxBox3dBody::setFriction(float friction) {
    // same note as density
}

void ofxBox3dBody::setRestitution(float restitution) {
    // same note
}

float ofxBox3dBody::getMass() const {
    if (!isValid()) return 0.f;
    return b3Body_GetMass(bodyId);
}

void ofxBox3dBody::setAwake(bool awake) {
    if (isValid()) b3Body_SetAwake(bodyId, awake);
}

bool ofxBox3dBody::isAwake() const {
    if (!isValid()) return false;
    return b3Body_IsAwake(bodyId);
}

void ofxBox3dBody::draw() const {
    if (!isValid()) return;
    ofPushStyle();
    ofSetColor(color);
    // Default: just draw a small axis / point – subclasses override
    ofDrawSphere(getPosition(), 0.1f * ofxBox3d::getScale());
    ofPopStyle();
}

b3ShapeDef ofxBox3dBody::makeShapeDef(float density, float friction, float restitution) const {
    b3ShapeDef def = b3DefaultShapeDef();
    def.density = density;
    def.baseMaterial.friction = friction;
    def.baseMaterial.restitution = restitution;
    return def;
}
