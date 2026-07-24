#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
    ofSetFrameRate(60);
    ofBackground(30);
    ofEnableDepthTest();

    // Optional: work in centimetres (100 OF units = 1 meter)
    // ofxBox3d::setScale(100.f);

    physics.setup({0.f, -9.81f, 0.f});

    // Static ground
    auto ground = std::make_shared<ofxBox3dBox>();
    ground->setup(physics, {0, -0.5f, 0}, {10.f, 0.5f, 10.f}, b3_staticBody);
    ground->setColor(ofColor(80, 80, 90));
    physics.addBody(ground);
    boxes.push_back(ground);

    // A few dynamic boxes
    for (int i = 0; i < 8; ++i) {
        auto box = std::make_shared<ofxBox3dBox>();
        float s = ofRandom(0.3f, 0.8f);
        glm::vec3 pos{ofRandom(-3, 3), 3.f + i * 1.2f, ofRandom(-3, 3)};
        box->setup(physics, pos, {s, s, s}, b3_dynamicBody, 1.f, 0.4f, 0.1f);
        box->setColor(ofColor::fromHsb(ofRandom(255), 180, 255));
        physics.addBody(box);
        boxes.push_back(box);
    }

    cam.setDistance(15);
    cam.setPosition(8, 6, 12);
    cam.lookAt({0, 1, 0});
}

//--------------------------------------------------------------
void ofApp::update() {
    physics.update();
}

//--------------------------------------------------------------
void ofApp::draw() {
    ofEnableDepthTest();
    cam.begin();

    ofDrawGrid(1.0f, 20, false, false, true, false); // XZ plane

    physics.draw();

    // Optional: draw axes at origin
    ofDrawAxis(1.0f);

    cam.end();
    ofDisableDepthTest();

    ofSetColor(255);
    ofDrawBitmapStringHighlight(
        "ofxBox3d basic example\n"
        "SPACE – drop a sphere\n"
        "R – reset\n"
        "bodies: " + ofToString(physics.getBodies().size()),
        20, 20);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
    if (key == ' ') {
        auto sph = std::make_shared<ofxBox3dSphere>();
        glm::vec3 pos{ofRandom(-2, 2), 8.f, ofRandom(-2, 2)};
        sph->setup(physics, pos, ofRandom(0.3f, 0.6f), b3_dynamicBody, 1.f, 0.2f, 0.4f);
        sph->setColor(ofColor::fromHsb(ofRandom(255), 200, 255));
        physics.addBody(sph);
        spheres.push_back(sph);
    }
    else if (key == 'r' || key == 'R') {
        physics.clear();
        boxes.clear();
        spheres.clear();
        setup(); // simple reset
    }
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {
    // future: ray-cast pick / throw
}
