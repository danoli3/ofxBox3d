#pragma once

#include "ofMain.h"
#include "ofxBox3dAll.h"

class ofApp : public ofBaseApp {
public:
    void setup() override;
    void update() override;
    void draw() override;
    void contactBegin(b3ContactBeginTouchEvent& event);

    ofEasyCam camera;
    ofxBox3d physics;
    std::shared_ptr<ofxBox3dBox> ground;
    std::vector<std::shared_ptr<ofxBox3dSphere>> spheres;
    int contactCount = 0;
};
