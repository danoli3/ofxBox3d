#include "ofMain.h"
#include "ofApp.h"

int main() {
    ofSetupOpenGL(1440, 900, OF_WINDOW);
    ofRunApp(std::make_shared<ofApp>());
}
