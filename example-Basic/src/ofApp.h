#pragma once

#include "ofMain.h"
#include "ofxBox3dAll.h"

class ofApp : public ofBaseApp {
public:
    void setup() override;
    void update() override;
    void draw() override;
    void keyPressed(int key) override;
    void mousePressed(int x, int y, int button) override;

    ofxBox3d physics;
    ofEasyCam cam;

    std::vector<std::shared_ptr<ofxBox3dBox>> boxes;
    std::vector<std::shared_ptr<ofxBox3dSphere>> spheres;
};
