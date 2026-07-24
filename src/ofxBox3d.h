#pragma once

#include "ofMain.h"
#include "ofxBox3dUtils.h"
#include <vector>
#include <memory>

// Forward
class ofxBox3dBody;

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
    void clear();                       // destroy world + all tracked bodies
    bool isSetup() const { return B3_IS_NON_NULL(worldId); }

    // -----------------------------------------------------------------
    // Simulation
    // -----------------------------------------------------------------
    /// Fixed-timestep update. Call once per frame.
    /// Internally accumulates time and steps at hz with subStepCount sub-steps.
    void update(float frameDt = -1.f);  // -1 → use ofGetLastFrameTime()

    void setFixedHz(float hz) { fixedHz = hz; }
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

    // -----------------------------------------------------------------
    // Events (contact start / end) – stub for now
    // -----------------------------------------------------------------
    ofEvent<void> contactStartEvent;   // TODO: richer args
    ofEvent<void> contactEndEvent;

protected:
    b3WorldId worldId = b3_nullWorldId;

    float fixedHz = 60.f;
    int   subStepCount = 4;
    float accumulator = 0.f;

    std::vector<std::shared_ptr<ofxBox3dBody>> bodies;
    bool bDrawBodies = true;

    void createWorld(const glm::vec3& gravity);
    void destroyWorld();
};
