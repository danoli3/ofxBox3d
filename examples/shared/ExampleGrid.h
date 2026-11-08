#pragma once

#include "ofMain.h"

/// Example-only grid controls. Use grid planes so alpha is respected;
/// ofDrawGrid replaces the supplied color (including its alpha).
struct ExampleGrid {
    bool visible = true;
    float opacity = 0.5f;

    void keyPressed(int key) {
        if (key == 'g' || key == 'G') visible = !visible;
        if (key == 'h' || key == 'H') opacity = opacity < 0.75f ? 1.f : 0.5f;
    }

    void draw(float step, int steps) const {
        if (!visible || opacity <= 0.f) return;
        ofPushStyle();
        ofEnableAlphaBlending();
        const unsigned char alpha = static_cast<unsigned char>(255.f * ofClamp(opacity, 0.f, 1.f));
        for (int plane = 0; plane < 3; ++plane) {
            ofSetColor(ofColor::fromHsb(plane * 85, 200, 255, alpha));
            ofPushMatrix();
            if (plane == 1) ofRotateZDeg(-90.f);
            if (plane == 2) ofRotateYDeg(90.f);
            ofDrawGridPlane(step, steps, false);
            ofPopMatrix();
        }
        ofPopStyle();
    }
};
