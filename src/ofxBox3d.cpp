#include "ofxBox3d.h"
#include "ofxBox3dBody.h"

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

void ofxBox3d::createWorld(const glm::vec3& gravity) {
    b3WorldDef def = b3DefaultWorldDef();
    def.gravity = ofxBox3dUtils::toB3(gravity);
    // Optional: def.enableSleep = true; etc.
    worldId = b3CreateWorld(&def);
    ofLogNotice("ofxBox3d") << "world created (id index=" << worldId.index1 << ")";
}

void ofxBox3d::destroyWorld() {
    if (B3_IS_NON_NULL(worldId)) {
        b3DestroyWorld(worldId);
        worldId = b3_nullWorldId;
        ofLogNotice("ofxBox3d") << "world destroyed";
    }
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
