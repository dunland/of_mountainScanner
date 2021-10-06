#include "consoleApp.h"
#include <iostream>
#include "Globals.h"

//--------------------------------------------------------------
void consoleApp::setup()
{
    gui.setup();
    gui.add(gui_imgThreshold.setup("Image Threshold", 113, 0, 255));
    gui.add(gui_canny_1.setup("Canny Threshold 1", 0, 0, 255));
    gui.add(gui_canny_2.setup("Canny Threshold 2", 0, 0, 255));
    gui.add(gui_edgeThreshold.setup("Edge Threshold", 50, 0, 100));
    gui.add(gui_lineThreshold.setup("Line Threshold", 150, 0, 200));
}

//--------------------------------------------------------------
void consoleApp::update()
{
    Controls::edgeThreshold = gui_edgeThreshold;
    Controls::lineThreshold = gui_lineThreshold;
    Controls::lowThreshold = gui_lowThreshold;
    Controls::img_threshold = gui_imgThreshold;
}

//--------------------------------------------------------------
void consoleApp::draw()
{
    ofBackground(0);

    // display test:
    string fpsStr = "test";
    ofDrawBitmapString(fpsStr, 20, 40);

    gui.draw();
}

//--------------------------------------------------------------
void consoleApp::keyPressed(int key)
{
}

//--------------------------------------------------------------
void consoleApp::keyReleased(int key)
{
}

//--------------------------------------------------------------
void consoleApp::mouseMoved(int x, int y)
{
}

//--------------------------------------------------------------
void consoleApp::mouseDragged(int x, int y, int button)
{
}

//--------------------------------------------------------------
void consoleApp::mousePressed(int x, int y, int button)
{
}

//--------------------------------------------------------------
void consoleApp::mouseReleased(int x, int y, int button)
{
}

//--------------------------------------------------------------
void consoleApp::mouseEntered(int x, int y)
{
}

//--------------------------------------------------------------
void consoleApp::mouseExited(int x, int y)
{
}

//--------------------------------------------------------------
void consoleApp::windowResized(int w, int h)
{
}

//--------------------------------------------------------------
void consoleApp::gotMessage(ofMessage msg)
{
}

//--------------------------------------------------------------
void consoleApp::dragEvent(ofDragInfo dragInfo)
{
}