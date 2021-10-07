#pragma once

#include "ofMain.h"
#include <vector>
#include <list>
#include "ofxGui.h"
#include "ofxOsc.h"
#include "settings.h"

class consoleApp : public ofBaseApp
{
public:
    void setup();
    void update();
    void draw();

    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y);
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseEntered(int x, int y);
    void mouseExited(int x, int y);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);

    ofxPanel gui;
    ofxIntSlider gui_imgThreshold;

    // hough lines:
    ofxIntSlider gui_edgeThreshold;
    ofxIntSlider gui_lineThreshold;
    ofxIntSlider gui_lowThreshold;

    // Canny Thresholds
    ofxIntSlider gui_canny_2;
    ofxIntSlider gui_canny_1;

    // Communication with Pd
    ofxToggle gui_send_button;
    ofxOscSender sender;
    ofxOscReceiver receiver;
};
