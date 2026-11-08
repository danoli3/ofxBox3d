#pragma once

#include "ofMain.h"
#include "ofxBox3dUtils.h"
#include "ofxBox3dBodySettings.h"
#include <algorithm>
#include <memory>
#include <vector>

// Forward
class ofxBox3dBody;
class ofxBox3dBox;
class ofxBox3dSphere;
class ofxBox3dCapsule;

/**
 * ofxBox3d – central physics world for openFrameworks + Box3D
 *
 * Typical usage:
 *   ofxBox3d physics;
 *   physics.setup();
 *   // create bodies...
 *   physics.update();
 *   physics.draw();
 */
class ofxBox3d {
public:
    ofxBox3d();
    ~ofxBox3d();

    // Non-copyable
    ofxBox3d(const ofxBox3d&) = delete;
    ofxBox3d& operator=(const ofxBox3d&) = delete;

    // -----------------------------------------------------------------
    // Lifecycle
    // -----------------------------------------------------------------
    void setup(const glm::vec3& gravity = {0.f, -10.f, 0.f});
    void setupWithDefinition(const b3WorldDef& definition);
    void clear();                       // destroy world + all tracked bodies
    bool isSetup() const { return B3_IS_NON_NULL(worldId) && b3World_IsValid(worldId); }

    /// Value-owned bodies: draw/sync them yourself; they are not in getBodies().
    ofxBox3dBox createBox(const glm::vec3& position, const glm::vec3& halfExtents,
                         const ofxBox3dBodySettings& settings = {});
    ofxBox3dBox createStaticBox(const glm::vec3& position, const glm::vec3& halfExtents,
                               float friction = 0.3f);
    ofxBox3dSphere createSphere(const glm::vec3& position, float radius,
                               const ofxBox3dBodySettings& settings = {});
    ofxBox3dCapsule createCapsule(const glm::vec3& position, float radius, float height,
                                 const ofxBox3dBodySettings& settings = {});
    int getBodyCount() const;
    int getAwakeBodyCount() const;

    // -----------------------------------------------------------------
    // Simulation
    // -----------------------------------------------------------------
    /// Fixed-timestep update. Call once per frame.
    /// Internally accumulates time and steps at hz with subStepCount sub-steps.
    void update(float frameDt = -1.f);  // -1 → use ofGetLastFrameTime()

    void setFixedHz(float hz) { fixedHz = std::max(hz, 1.f); }
    float getFixedHz() const { return fixedHz; }

    void setSubStepCount(int count) { subStepCount = std::max(1, count); }
    int  getSubStepCount() const { return subStepCount; }

    // -----------------------------------------------------------------
    // Gravity & world parameters
    // -----------------------------------------------------------------
    void setGravity(const glm::vec3& g);
    glm::vec3 getGravity() const;

    b3WorldId getWorldId() const { return worldId; }

    // -----------------------------------------------------------------
    // Body bookkeeping (optional – bodies can live outside this list)
    // -----------------------------------------------------------------
    void addBody(std::shared_ptr<ofxBox3dBody> body);
    void removeBody(std::shared_ptr<ofxBox3dBody> body);
    void clearBodies();

    const std::vector<std::shared_ptr<ofxBox3dBody>>& getBodies() const { return bodies; }

    // -----------------------------------------------------------------
    // Debug drawing
    // -----------------------------------------------------------------
    void draw() const;
    void setDrawBodies(bool b) { bDrawBodies = b; }
    bool getDrawBodies() const { return bDrawBodies; }

    ofEvent<b3ContactBeginTouchEvent> contactBeginEvent;
    ofEvent<b3ContactEndTouchEvent> contactEndEvent;
    ofEvent<b3ContactHitEvent> contactHitEvent;
    ofEvent<b3SensorBeginTouchEvent> sensorBeginEvent;
    ofEvent<b3SensorEndTouchEvent> sensorEndEvent;

protected:
    b3WorldId worldId = b3_nullWorldId;

    float fixedHz = 60.f;
    int   subStepCount = 4;
    float accumulator = 0.f;

    std::vector<std::shared_ptr<ofxBox3dBody>> bodies;
    bool bDrawBodies = true;

    void createWorld(const glm::vec3& gravity);
    void destroyWorld();
    void notifyEvents();
};
