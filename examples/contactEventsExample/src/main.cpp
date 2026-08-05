#include "ofMain.h"
#include "ofApp.h"

int main() {
    ofSetupOpenGL(1280, 720, OF_WINDOW);
    ofRunApp(std::make_shared<ofApp>());
}
