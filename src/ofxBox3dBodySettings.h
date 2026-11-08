#pragma once

#include "ofxBox3dUtils.h"

/// Creation settings shared by boxes, spheres, capsules, and empty bodies.
struct ofxBox3dBodySettings {
    b3BodyType type = b3_dynamicBody;
    float density = 1.f;
    float friction = 0.3f;
    float restitution = 0.f;
    float linearDamping = 0.f;
    float angularDamping = 0.f;
    float gravityScale = 1.f;
    bool enableSleep = true;
    bool isBullet = false;
    bool isSensor = false;
    bool enableContactEvents = false;
    bool enableHitEvents = false;
    bool enableSensorEvents = false;
    void* userData = nullptr;
    ofQuaternion rotation;
};
