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

    // Physics ownership, local transform, style, and parent link transfer.
    // Children stay attached to the original node; keep tree nodes at stable addresses.
    ofxBox3dBody(ofxBox3dBody&& other);
    ofxBox3dBody& operator=(ofxBox3dBody&& other);

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

    bool setup(ofxBox3d& world, const glm::vec3& position,
               const ofxBox3dBodySettings& settings);

    // -----------------------------------------------------------------
    // Lifetime
    // -----------------------------------------------------------------
    void destroy();
    bool isValid() const {
        return B3_IS_NON_NULL(bodyId) && b3World_IsValid(ownerWorldId) && b3Body_IsValid(bodyId);
    }
    b3BodyId getBodyId() const { return bodyId; }

    // -----------------------------------------------------------------
    // Transform (synced from / to physics)
    // -----------------------------------------------------------------
    void syncFromPhysics();                 // body → ofNode
    void syncToPhysics();                   // ofNode → body (teleport)

    glm::vec3 getPosition() const;
    glm::dvec3 getPositionDouble() const;
    ofQuaternion getOrientationQuat() const;
    void setPosition(const glm::vec3& p);
    void setPositionDouble(const glm::dvec3& p);
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
    void applyLinearImpulseToCenter(const glm::vec3& impulse);
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
    void setLinearDamping(float damping);
    float getLinearDamping() const;
    void setAngularDamping(float damping);
    float getAngularDamping() const;
    void setGravityScale(float scale);
    float getGravityScale() const;
    void setBullet(bool bullet);
    bool isBullet() const;
    void setEnabled(bool enabled);
    bool isEnabled() const;
    void setAwake(bool awake);
    bool isAwake() const;

    // -----------------------------------------------------------------
    // Drawing (override in subclasses for better shapes)
    // -----------------------------------------------------------------
    virtual void draw() const;
    void setColor(const ofColor& c) { color = c; }
    ofColor getColor() const { return color; }
    void setFilled(bool filled) { bFilled = filled; }
    bool isFilled() const { return bFilled; }

    void enableContactEvents(bool enabled = true);
    void enableHitEvents(bool enabled = true);
    void enableSensorEvents(bool enabled = true);

protected:
    b3BodyId bodyId = b3_nullBodyId;
    // Save the world's generation as well as the body ID: world slots can be reused.
    b3WorldId ownerWorldId = b3_nullWorldId;
    ofColor color = ofColor::white;
    bool bFilled = true;

    // Helper used by derived classes when creating shapes
    b3ShapeDef makeShapeDef(float density = 1.f,
                            float friction = 0.3f,
                            float restitution = 0.f) const;
    b3ShapeDef makeShapeDef(const ofxBox3dBodySettings& settings) const;
    glm::mat4 getRenderTransformMatrix() const;

private:
    void moveFrom(ofxBox3dBody& other);
};
