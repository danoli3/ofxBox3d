#include "ofApp.h"

void ofApp::setup() {
    ofxBox3dUtils::setScale(100.f);
    physics.setup({0.f, -981.f, 0.f});
    ofAddListener(physics.contactBeginEvent, this, &ofApp::contactBegin);

    ground = std::make_shared<ofxBox3dBox>();
    ground->setup(physics, {0.f, -100.f, 0.f}, {800.f, 50.f, 800.f}, b3_staticBody);
    ground->setColor(ofColor::darkGray);
    physics.addBody(ground);

    for (int i = 0; i < 20; ++i) {
        auto sphere = std::make_shared<ofxBox3dSphere>();
        sphere->setup(physics, {ofRandom(-300.f, 300.f), 200.f + i * 100.f, ofRandom(-300.f, 300.f)}, 45.f);
        sphere->enableContactEvents();
        sphere->setColor(ofColor::orange);
        physics.addBody(sphere);
        spheres.push_back(sphere);
    }
    camera.setDistance(2200.f);
}

void ofApp::update() {
    physics.update();
}

void ofApp::draw() {
    ofBackground(18);
    ofEnableDepthTest();
    camera.begin();
    physics.draw();
    camera.end();
    ofDisableDepthTest();
    ofDrawBitmapStringHighlight("contact begins: " + ofToString(contactCount), 20, 24);
}

void ofApp::contactBegin(b3ContactBeginTouchEvent&) {
    ++contactCount;
}
