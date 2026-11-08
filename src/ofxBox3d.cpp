#include "ofxBox3d.h"
#include "ofxBox3dBody.h"
#include "ofxBox3dBox.h"
#include "ofxBox3dSphere.h"
#include "ofxBox3dCapsule.h"

ofxBox3d::ofxBox3d() = default;

ofxBox3d::~ofxBox3d() {
    clear();
}

void ofxBox3d::setup(const glm::vec3& gravity) {
    if (isSetup()) {
        ofLogWarning("ofxBox3d") << "already set up – clearing first";
        clear();
    }
    createWorld(gravity);
}

void ofxBox3d::clear() {
    clearBodies();
    destroyWorld();
    accumulator = 0.f;
}

void ofxBox3d::setupWithDefinition(const b3WorldDef& definition) {
    clear();
    worldId = b3CreateWorld(&definition);
}

ofxBox3dBox ofxBox3d::createBox(const glm::vec3& position, const glm::vec3& halfExtents,
                               const ofxBox3dBodySettings& settings) {
    if (!isSetup()) setup();
    ofxBox3dBox body;
    body.setup(*this, position, halfExtents, settings);
    return body;
}

ofxBox3dBox ofxBox3d::createStaticBox(const glm::vec3& position, const glm::vec3& halfExtents,
                                     float friction) {
    ofxBox3dBodySettings settings;
    settings.type = b3_staticBody;
    settings.density = 0.f;
    settings.friction = friction;
    return createBox(position, halfExtents, settings);
}

ofxBox3dSphere ofxBox3d::createSphere(const glm::vec3& position, float radius,
                                     const ofxBox3dBodySettings& settings) {
    if (!isSetup()) setup();
    ofxBox3dSphere body;
    body.setup(*this, position, radius, settings);
    return body;
}

ofxBox3dCapsule ofxBox3d::createCapsule(const glm::vec3& position, float radius, float height,
                                       const ofxBox3dBodySettings& settings) {
    if (!isSetup()) setup();
    ofxBox3dCapsule body;
    body.setup(*this, position, radius, height, settings);
    return body;
}

int ofxBox3d::getBodyCount() const {
    return isSetup() ? b3World_GetCounters(worldId).bodyCount : 0;
}

int ofxBox3d::getAwakeBodyCount() const {
    return isSetup() ? b3World_GetAwakeBodyCount(worldId) : 0;
}

void ofxBox3d::createWorld(const glm::vec3& gravity) {
    b3WorldDef def = b3DefaultWorldDef();
    def.gravity = ofxBox3dUtils::toB3(gravity);
    // Optional: def.enableSleep = true; etc.
    worldId = b3CreateWorld(&def);
    ofLogNotice("ofxBox3d") << "world created (id index=" << worldId.index1 << ")";
}

void ofxBox3d::destroyWorld() {
    if (isSetup()) {
        b3DestroyWorld(worldId);
        ofLogNotice("ofxBox3d") << "world destroyed";
    }
    worldId = b3_nullWorldId;
}

void ofxBox3d::update(float frameDt) {
    if (!isSetup()) return;

    float dt = (frameDt < 0.f) ? ofGetLastFrameTime() : frameDt;
    // Clamp to avoid spiral of death
    dt = std::min(dt, 0.1f);

    accumulator += dt;
    const float step = 1.f / fixedHz;

    while (accumulator >= step) {
        b3World_Step(worldId, step, subStepCount);
        notifyEvents();
        accumulator -= step;
    }

    // Sync ofNode transforms for all tracked bodies
    for (auto& b : bodies) {
        if (b) b->syncFromPhysics();
    }
}

void ofxBox3d::notifyEvents() {
    b3ContactEvents contacts = b3World_GetContactEvents(worldId);
    for (int i = 0; i < contacts.beginCount; ++i) {
        ofNotifyEvent(contactBeginEvent, contacts.beginEvents[i], this);
    }
    for (int i = 0; i < contacts.endCount; ++i) {
        ofNotifyEvent(contactEndEvent, contacts.endEvents[i], this);
    }
    for (int i = 0; i < contacts.hitCount; ++i) {
        ofNotifyEvent(contactHitEvent, contacts.hitEvents[i], this);
    }

    b3SensorEvents sensors = b3World_GetSensorEvents(worldId);
    for (int i = 0; i < sensors.beginCount; ++i) {
        ofNotifyEvent(sensorBeginEvent, sensors.beginEvents[i], this);
    }
    for (int i = 0; i < sensors.endCount; ++i) {
        ofNotifyEvent(sensorEndEvent, sensors.endEvents[i], this);
    }
}

void ofxBox3d::setGravity(const glm::vec3& g) {
    if (!isSetup()) return;
    b3World_SetGravity(worldId, ofxBox3dUtils::toB3(g));
}

glm::vec3 ofxBox3d::getGravity() const {
    if (!isSetup()) return {0,0,0};
    return ofxBox3dUtils::toOf(b3World_GetGravity(worldId));
}

void ofxBox3d::addBody(std::shared_ptr<ofxBox3dBody> body) {
    if (!body) return;
    bodies.push_back(body);
}

void ofxBox3d::removeBody(std::shared_ptr<ofxBox3dBody> body) {
    bodies.erase(std::remove(bodies.begin(), bodies.end(), body), bodies.end());
}

void ofxBox3d::clearBodies() {
    // Bodies own their b3BodyId and will destroy it in their destructor
    bodies.clear();
}

void ofxBox3d::draw() const {
    if (!bDrawBodies) return;
    for (const auto& b : bodies) {
        if (b) b->draw();
    }
}
