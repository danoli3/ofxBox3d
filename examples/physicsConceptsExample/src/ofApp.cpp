#include "ofApp.h"

namespace {
const char* sceneNames[] = {
    "Gravity and stacking",
    "Friction",
    "Restitution",
    "Static, dynamic, kinematic",
    "Forces and impulses",
    "Continuous collision",
    "Distance joint"
};

ofColor colors[] = {
    ofColor(80, 170, 240),
    ofColor(245, 170, 55),
    ofColor(120, 210, 130),
    ofColor(220, 100, 145)
};
}

void ofApp::setup() {
    ofSetVerticalSync(true);
    ofSetFrameRate(60);
    ofxBox3dUtils::setScale(100.f);
    gui.setup(nullptr, true);
    light.setup();
    light.setPosition(600.f, 1000.f, 800.f);
    camera.setDistance(1800.f);
    loadScene(0);
}

std::shared_ptr<ofxBox3dBox> ofApp::addBox(const glm::vec3& position,
                                           const glm::vec3& halfExtents,
                                           b3BodyType type) {
    auto body = std::make_shared<ofxBox3dBox>();
    body->setup(physics, position, halfExtents, type);
    body->setColor(colors[bodies.size() % 4]);
    physics.addBody(body);
    bodies.push_back(body);
    return body;
}

std::shared_ptr<ofxBox3dSphere> ofApp::addSphere(const glm::vec3& position,
                                                 float radius,
                                                 b3BodyType type) {
    auto body = std::make_shared<ofxBox3dSphere>();
    body->setup(physics, position, radius, type);
    body->setColor(colors[bodies.size() % 4]);
    physics.addBody(body);
    bodies.push_back(body);
    return body;
}

void ofApp::addGround() {
    auto ground = addBox({0.f, -100.f, 0.f}, {700.f, 50.f, 700.f}, b3_staticBody);
    ground->setColor(ofColor(75));
}

void ofApp::loadScene(int index) {
    sceneIndex = ofClamp(index, 0, 6);
    bodies.clear();
    controlledBody.reset();
    kinematicBody.reset();
    jointId = b3_nullJointId;
    physics.setup({0.f, -981.f, 0.f});

    if (sceneIndex == 0) {
        addGround();
        for (int y = 0; y < 7; ++y) {
            for (int x = -2; x <= 2; ++x) {
                addBox({x * 105.f, 50.f + y * 105.f, 0.f}, {50.f, 50.f, 50.f});
            }
        }
    } else if (sceneIndex == 1) {
        addGround();
        for (int i = 0; i < 6; ++i) {
            auto box = addBox({-500.f + i * 200.f, 100.f, 0.f}, {70.f, 70.f, 70.f});
            box->setFriction(i * 0.2f);
            box->applyLinearImpulseToCenter({500.f, 0.f, 0.f});
        }
    } else if (sceneIndex == 2) {
        addGround();
        for (int i = 0; i < 6; ++i) {
            auto sphere = addSphere({-500.f + i * 200.f, 700.f, 0.f}, 65.f);
            sphere->setRestitution(i * 0.2f);
        }
    } else if (sceneIndex == 3) {
        addGround();
        auto staticBody = addBox({-350.f, 100.f, 0.f}, {100.f, 100.f, 100.f}, b3_staticBody);
        staticBody->setColor(ofColor::gray);
        addBox({0.f, 500.f, 0.f}, {100.f, 100.f, 100.f});
        kinematicBody = addBox({350.f, 150.f, 0.f}, {180.f, 35.f, 180.f}, b3_kinematicBody);
        kinematicBody->setLinearVelocity({0.f, 180.f, 0.f});
    } else if (sceneIndex == 4) {
        addGround();
        controlledBody = addBox({0.f, 350.f, 0.f}, {100.f, 100.f, 100.f});
        controlledBody->setColor(ofColor::yellow);
    } else if (sceneIndex == 5) {
        addBox({300.f, 300.f, 0.f}, {15.f, 400.f, 400.f}, b3_staticBody)->setColor(ofColor::gray);
        controlledBody = addSphere({-550.f, 300.f, 0.f}, 35.f);
        controlledBody->setBullet(true);
        controlledBody->setLinearVelocity({3500.f, 0.f, 0.f});
    } else if (sceneIndex == 6) {
        addGround();
        auto anchor = addSphere({0.f, 650.f, 0.f}, 35.f, b3_staticBody);
        anchor->setColor(ofColor::white);
        controlledBody = addSphere({0.f, 250.f, 0.f}, 80.f);
        b3DistanceJointDef def = b3DefaultDistanceJointDef();
        def.base.bodyIdA = anchor->getBodyId();
        def.base.bodyIdB = controlledBody->getBodyId();
        def.length = 4.f;
        def.enableSpring = true;
        def.hertz = 2.f;
        def.dampingRatio = 0.2f;
        jointId = b3CreateDistanceJoint(physics.getWorldId(), &def);
        controlledBody->applyLinearImpulseToCenter({350.f, 0.f, 0.f});
    }
}

void ofApp::update() {
    if (kinematicBody) {
        float y = kinematicBody->getPosition().y;
        if (y > 500.f) kinematicBody->setLinearVelocity({0.f, -180.f, 0.f});
        if (y < 100.f) kinematicBody->setLinearVelocity({0.f, 180.f, 0.f});
    }
    if (!paused || stepOnce) physics.update(1.f / 60.f);
    stepOnce = false;
}

void ofApp::draw() {
    ofBackground(24);
    ofEnableDepthTest();
    camera.begin();
    light.enable();
    physics.draw();
    light.disable();
    if (B3_IS_NON_NULL(jointId) && b3Joint_IsValid(jointId) && controlledBody) {
        ofSetColor(ofColor::white);
        ofSetLineWidth(3.f);
        ofDrawLine({0.f, 650.f, 0.f}, controlledBody->getPosition());
    }
    ofSetColor(90);
    ofDrawGrid(100.f, 14, true);
    camera.end();
    ofDisableDepthTest();

    gui.begin();
    ImGui::SetNextWindowSize(ImVec2(330, 0), ImGuiCond_Once);
    ImGui::Begin("Box3D physics concepts");
    if (ImGui::BeginCombo("Scene", sceneNames[sceneIndex])) {
        for (int i = 0; i < 7; ++i) {
            if (ImGui::Selectable(sceneNames[i], sceneIndex == i)) loadScene(i);
        }
        ImGui::EndCombo();
    }
    if (ImGui::Button("Reset")) loadScene(sceneIndex);
    ImGui::SameLine();
    ImGui::Checkbox("Pause", &paused);
    ImGui::SameLine();
    if (ImGui::Button("Step")) stepOnce = true;
    ImGui::Text("Bodies: %d", static_cast<int>(bodies.size()));
    ImGui::Text("Awake: %d", b3World_GetAwakeBodyCount(physics.getWorldId()));
    if (sceneIndex == 4 && controlledBody) {
        ImGui::SeparatorText("Apply to yellow box");
        if (ImGui::Button("Force up")) controlledBody->applyForceToCenter({0.f, 8000.f, 0.f});
        if (ImGui::Button("Impulse left")) controlledBody->applyLinearImpulseToCenter({-500.f, 0.f, 0.f});
        ImGui::SameLine();
        if (ImGui::Button("Impulse right")) controlledBody->applyLinearImpulseToCenter({500.f, 0.f, 0.f});
        if (ImGui::Button("Torque")) controlledBody->applyTorque({0.f, 0.f, 800.f});
    }
    ImGui::Separator();
    ImGui::TextDisabled("1-7 scenes  R reset  P pause  N step");
    ImGui::End();
    gui.end();
}

void ofApp::keyPressed(int key) {
    if (key >= '1' && key <= '7') loadScene(key - '1');
    if (key == 'r' || key == 'R') loadScene(sceneIndex);
    if (key == 'p' || key == 'P') paused = !paused;
    if (key == 'n' || key == 'N') stepOnce = true;
}
