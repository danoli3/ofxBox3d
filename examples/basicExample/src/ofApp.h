#pragma once

#include "ofMain.h"
#include "ofxBox3dAll.h"

class ofApp : public ofBaseApp {
public:
    void setup() override;
    void update() override;
    void draw() override;
    void keyPressed(int key) override;

    void spawn();

    ofEasyCam camera;
    ofxBox3d physics;
    std::vector<std::shared_ptr<ofxBox3dBody>> actors;
};
