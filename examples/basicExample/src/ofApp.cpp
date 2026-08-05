#include "ofApp.h"

void ofApp::setup() {
    ofSetVerticalSync(true);
    ofSetFrameRate(60);
    ofxBox3dUtils::setScale(100.f);
    physics.setup({0.f, -981.f, 0.f});

    auto ground = std::make_shared<ofxBox3dBox>();
    ground->setup(physics, {0.f, -100.f, 0.f}, {800.f, 50.f, 800.f}, b3_staticBody);
    ground->setColor(ofColor::darkGray);
    actors.push_back(ground);
    physics.addBody(ground);

    for (int i = 0; i < 12; ++i) spawn();
    camera.setDistance(1800.f);
}

void ofApp::spawn() {
    std::shared_ptr<ofxBox3dBody> actor;
    glm::vec3 position{ofRandom(-200.f, 200.f), ofRandom(400.f, 1000.f), ofRandom(-200.f, 200.f)};
    int shape = actors.size() % 3;
    if (shape == 0) {
        auto box = std::make_shared<ofxBox3dBox>();
        box->setup(physics, position, {50.f, 50.f, 50.f});
        actor = box;
    } else if (shape == 1) {
        auto sphere = std::make_shared<ofxBox3dSphere>();
        sphere->setup(physics, position, 55.f, b3_dynamicBody, 1.f, 0.4f, 0.3f);
        actor = sphere;
    } else {
        auto capsule = std::make_shared<ofxBox3dCapsule>();
        capsule->setup(physics, position, 40.f, 160.f);
        actor = capsule;
    }
    actor->setColor(ofColor::fromHsb(ofRandom(255), 180, 230));
    actors.push_back(actor);
    physics.addBody(actor);
}

void ofApp::update() {
    physics.update();
}

void ofApp::draw() {
    ofBackground(22);
    ofEnableDepthTest();
    camera.begin();
    ofDrawGrid(100.f, 16, true);
    physics.draw();
    camera.end();
    ofDisableDepthTest();
    ofDrawBitmapStringHighlight("space: spawn   r: reset", 20, 24);
}

void ofApp::keyPressed(int key) {
    if (key == ' ') spawn();
    if (key == 'r') {
        actors.clear();
        setup();
    }
}
