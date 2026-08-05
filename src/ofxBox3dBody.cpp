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
    def.position = ofxBox3dUtils::toB3(position);
    def.rotation = ofxBox3dUtils::toB3(rotation);

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
    ofNode::setPosition(ofxBox3dUtils::toOf(p));
    ofNode::setOrientation(ofxBox3dUtils::toOf(q));
}

void ofxBox3dBody::syncToPhysics() {
    if (!isValid()) return;
    b3Body_SetTransform(bodyId,
                        ofxBox3dUtils::toB3(ofNode::getPosition()),
                        ofxBox3dUtils::toB3(ofNode::getOrientationQuat()));
}

glm::vec3 ofxBox3dBody::getPosition() const {
    if (!isValid()) return ofNode::getPosition();
    return ofxBox3dUtils::toOf(b3Body_GetPosition(bodyId));
}

ofQuaternion ofxBox3dBody::getOrientationQuat() const {
    if (!isValid()) return ofNode::getOrientationQuat();
    return ofxBox3dUtils::toOf(b3Body_GetRotation(bodyId));
}

void ofxBox3dBody::setPosition(const glm::vec3& p) {
    ofNode::setPosition(p);
    if (isValid()) {
        b3Body_SetTransform(bodyId, ofxBox3dUtils::toB3(p), b3Body_GetRotation(bodyId));
    }
}

void ofxBox3dBody::setOrientation(const ofQuaternion& q) {
    ofNode::setOrientation(q);
    if (isValid()) {
        b3Body_SetTransform(bodyId, b3Body_GetPosition(bodyId), ofxBox3dUtils::toB3(q));
    }
}

void ofxBox3dBody::setTransform(const glm::vec3& p, const ofQuaternion& q) {
    ofNode::setPosition(p);
    ofNode::setOrientation(q);
    if (isValid()) {
        b3Body_SetTransform(bodyId, ofxBox3dUtils::toB3(p), ofxBox3dUtils::toB3(q));
    }
}

void ofxBox3dBody::setLinearVelocity(const glm::vec3& v) {
    if (isValid()) b3Body_SetLinearVelocity(bodyId, ofxBox3dUtils::toB3(v));
}

glm::vec3 ofxBox3dBody::getLinearVelocity() const {
    if (!isValid()) return {0,0,0};
    return ofxBox3dUtils::toOf(b3Body_GetLinearVelocity(bodyId));
}

void ofxBox3dBody::setAngularVelocity(const glm::vec3& w) {
    if (isValid()) b3Body_SetAngularVelocity(bodyId, ofxBox3dUtils::toB3(w));
}

glm::vec3 ofxBox3dBody::getAngularVelocity() const {
    if (!isValid()) return {0,0,0};
    return ofxBox3dUtils::toOf(b3Body_GetAngularVelocity(bodyId));
}

void ofxBox3dBody::applyForce(const glm::vec3& force, const glm::vec3& point) {
    if (isValid()) b3Body_ApplyForce(bodyId, ofxBox3dUtils::toB3(force), ofxBox3dUtils::toB3(point), true);
}

void ofxBox3dBody::applyForceToCenter(const glm::vec3& force) {
    if (isValid()) b3Body_ApplyForceToCenter(bodyId, ofxBox3dUtils::toB3(force), true);
}

void ofxBox3dBody::applyTorque(const glm::vec3& torque) {
    if (isValid()) b3Body_ApplyTorque(bodyId, ofxBox3dUtils::toB3(torque), true);
}

void ofxBox3dBody::applyLinearImpulse(const glm::vec3& impulse, const glm::vec3& point) {
    if (isValid()) b3Body_ApplyLinearImpulse(bodyId, ofxBox3dUtils::toB3(impulse), ofxBox3dUtils::toB3(point), true);
}

void ofxBox3dBody::applyLinearImpulseToCenter(const glm::vec3& impulse) {
    if (isValid()) b3Body_ApplyLinearImpulseToCenter(bodyId, ofxBox3dUtils::toB3(impulse), true);
}

void ofxBox3dBody::applyAngularImpulse(const glm::vec3& impulse) {
    if (isValid()) b3Body_ApplyAngularImpulse(bodyId, ofxBox3dUtils::toB3(impulse), true);
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
    std::vector<b3ShapeId> shapes(b3Body_GetShapeCount(bodyId));
    int count = b3Body_GetShapes(bodyId, shapes.data(), static_cast<int>(shapes.size()));
    for (int i = 0; i < count; ++i) b3Shape_SetDensity(shapes[i], density, false);
    b3Body_ApplyMassFromShapes(bodyId);
}

void ofxBox3dBody::setFriction(float friction) {
    if (!isValid()) return;
    std::vector<b3ShapeId> shapes(b3Body_GetShapeCount(bodyId));
    int count = b3Body_GetShapes(bodyId, shapes.data(), static_cast<int>(shapes.size()));
    for (int i = 0; i < count; ++i) b3Shape_SetFriction(shapes[i], friction);
}

void ofxBox3dBody::setRestitution(float restitution) {
    if (!isValid()) return;
    std::vector<b3ShapeId> shapes(b3Body_GetShapeCount(bodyId));
    int count = b3Body_GetShapes(bodyId, shapes.data(), static_cast<int>(shapes.size()));
    for (int i = 0; i < count; ++i) b3Shape_SetRestitution(shapes[i], restitution);
}

void ofxBox3dBody::setLinearDamping(float damping) { if (isValid()) b3Body_SetLinearDamping(bodyId, damping); }
float ofxBox3dBody::getLinearDamping() const { return isValid() ? b3Body_GetLinearDamping(bodyId) : 0.f; }
void ofxBox3dBody::setAngularDamping(float damping) { if (isValid()) b3Body_SetAngularDamping(bodyId, damping); }
float ofxBox3dBody::getAngularDamping() const { return isValid() ? b3Body_GetAngularDamping(bodyId) : 0.f; }
void ofxBox3dBody::setGravityScale(float scale) { if (isValid()) b3Body_SetGravityScale(bodyId, scale); }
float ofxBox3dBody::getGravityScale() const { return isValid() ? b3Body_GetGravityScale(bodyId) : 0.f; }
void ofxBox3dBody::setBullet(bool bullet) { if (isValid()) b3Body_SetBullet(bodyId, bullet); }
bool ofxBox3dBody::isBullet() const { return isValid() && b3Body_IsBullet(bodyId); }
void ofxBox3dBody::setEnabled(bool enabled) {
    if (!isValid() || enabled == b3Body_IsEnabled(bodyId)) return;
    if (enabled) b3Body_Enable(bodyId); else b3Body_Disable(bodyId);
}
bool ofxBox3dBody::isEnabled() const { return isValid() && b3Body_IsEnabled(bodyId); }

void ofxBox3dBody::enableContactEvents(bool enabled) {
    if (!isValid()) return;
    std::vector<b3ShapeId> shapes(b3Body_GetShapeCount(bodyId));
    int count = b3Body_GetShapes(bodyId, shapes.data(), static_cast<int>(shapes.size()));
    for (int i = 0; i < count; ++i) b3Shape_EnableContactEvents(shapes[i], enabled);
}

void ofxBox3dBody::enableHitEvents(bool enabled) {
    if (!isValid()) return;
    std::vector<b3ShapeId> shapes(b3Body_GetShapeCount(bodyId));
    int count = b3Body_GetShapes(bodyId, shapes.data(), static_cast<int>(shapes.size()));
    for (int i = 0; i < count; ++i) b3Shape_EnableHitEvents(shapes[i], enabled);
}

void ofxBox3dBody::enableSensorEvents(bool enabled) {
    if (!isValid()) return;
    std::vector<b3ShapeId> shapes(b3Body_GetShapeCount(bodyId));
    int count = b3Body_GetShapes(bodyId, shapes.data(), static_cast<int>(shapes.size()));
    for (int i = 0; i < count; ++i) b3Shape_EnableSensorEvents(shapes[i], enabled);
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
    ofDrawSphere(getPosition(), 0.1f * ofxBox3dUtils::getScale());
    ofPopStyle();
}

b3ShapeDef ofxBox3dBody::makeShapeDef(float density, float friction, float restitution) const {
    b3ShapeDef def = b3DefaultShapeDef();
    def.density = density;
    def.baseMaterial.friction = friction;
    def.baseMaterial.restitution = restitution;
    return def;
}
