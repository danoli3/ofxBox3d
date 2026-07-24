#pragma once

#include "ofxBox3dUtils.h"
#include "ofxBox3d.h"

/**
 * Base class for physics bodies.
 * Holds a b3BodyId and can optionally own an ofNode for easy rendering.
 *
 * Derive concrete shapes (ofxBox3dBox, ofxBox3dSphere, …) or use directly
 * after attaching shapes via the low-level Box3D API.
 */
class ofxBox3dBody : public ofNode {
public:
    ofxBox3dBody() = default;
    virtual ~ofxBox3dBody();

    // Non-copyable (IDs are unique)
    ofxBox3dBody(const ofxBox3dBody&) = delete;
    ofxBox3dBody& operator=(const ofxBox3dBody&) = delete;

    // -----------------------------------------------------------------
    // Creation helpers (call after world is ready)
    // -----------------------------------------------------------------
    /// Create an empty body (you must attach shapes yourself)
    bool setup(ofxBox3d& world,
               const glm::vec3& position = {0,0,0},
               b3BodyType type = b3_dynamicBody);

    /// Convenience: create + set density/friction later via setDensity etc.
    bool setup(ofxBox3d& world,
               const glm::vec3& position,
               const ofQuaternion& rotation,
               b3BodyType type = b3_dynamicBody);

    // -----------------------------------------------------------------
    // Lifetime
    // -----------------------------------------------------------------
    void destroy();
    bool isValid() const { return B3_IS_NON_NULL(bodyId) && b3Body_IsValid(bodyId); }
    b3BodyId getBodyId() const { return bodyId; }

    // -----------------------------------------------------------------
    // Transform (synced from / to physics)
    // -----------------------------------------------------------------
    void syncFromPhysics();                 // body → ofNode
    void syncToPhysics();                   // ofNode → body (teleport)

    glm::vec3 getPosition() const;
    ofQuaternion getOrientationQuat() const;
    void setPosition(const glm::vec3& p);
    void setOrientation(const ofQuaternion& q);
    void setTransform(const glm::vec3& p, const ofQuaternion& q);

    // -----------------------------------------------------------------
    // Dynamics
    // -----------------------------------------------------------------
    void setLinearVelocity(const glm::vec3& v);
    glm::vec3 getLinearVelocity() const;
    void setAngularVelocity(const glm::vec3& w);
    glm::vec3 getAngularVelocity() const;

    void applyForce(const glm::vec3& force, const glm::vec3& point);
    void applyForceToCenter(const glm::vec3& force);
    void applyTorque(const glm::vec3& torque);
    void applyLinearImpulse(const glm::vec3& impulse, const glm::vec3& point);
    void applyAngularImpulse(const glm::vec3& impulse);

    // -----------------------------------------------------------------
    // Body properties
    // -----------------------------------------------------------------
    void setType(b3BodyType type);
    b3BodyType getType() const;

    void setStatic();
    void setDynamic();
    void setKinematic();

    void setDensity(float density);          // applied to all shapes on the body
    void setFriction(float friction);
    void setRestitution(float restitution);

    float getMass() const;
    void setAwake(bool awake);
    bool isAwake() const;

    // -----------------------------------------------------------------
    // Drawing (override in subclasses for better shapes)
    // -----------------------------------------------------------------
    virtual void draw() const;
    void setColor(const ofColor& c) { color = c; }
    ofColor getColor() const { return color; }

protected:
    b3BodyId bodyId = b3_nullBodyId;
    ofxBox3d* worldPtr = nullptr;           // non-owning
    ofColor color = ofColor::white;

    // Helper used by derived classes when creating shapes
    b3ShapeDef makeShapeDef(float density = 1.f,
                            float friction = 0.3f,
                            float restitution = 0.f) const;
};
