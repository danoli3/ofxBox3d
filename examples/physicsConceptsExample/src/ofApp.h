#pragma once

#include "ofMain.h"
#include "ofxBox3dAll.h"
#include "ofxImGui.h"

class ofApp : public ofBaseApp {
public:
    void setup() override;
    void update() override;
    void draw() override;
    void keyPressed(int key) override;

    void loadScene(int index);
    void addGround();
    std::shared_ptr<ofxBox3dBox> addBox(const glm::vec3& position,
                                       const glm::vec3& halfExtents,
                                       b3BodyType type = b3_dynamicBody);
    std::shared_ptr<ofxBox3dSphere> addSphere(const glm::vec3& position,
                                             float radius,
                                             b3BodyType type = b3_dynamicBody);

    ofEasyCam camera;
    ofLight light;
    ofxImGui::Gui gui;
    ofxBox3d physics;
    std::vector<std::shared_ptr<ofxBox3dBody>> bodies;
    std::shared_ptr<ofxBox3dBody> controlledBody;
    std::shared_ptr<ofxBox3dBox> kinematicBody;
    b3JointId jointId = b3_nullJointId;
    int sceneIndex = 0;
    bool paused = false;
    bool stepOnce = false;
};
