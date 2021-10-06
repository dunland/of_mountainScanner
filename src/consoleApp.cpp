#include "consoleApp.h"
#include <iostream>
#include "Globals.h"

//--------------------------------------------------------------
void consoleApp::setup()
{
    gui.setup();
    gui.add(Controls::edgeThreshold.setup("Edge Threshold", 50, 0, 100));
    gui.add(Controls::lineThreshold.setup("Line Threshold", 150, 0, 200));
}

//--------------------------------------------------------------
void consoleApp::update()
{
}

//--------------------------------------------------------------
void consoleApp::draw()
{
    ofBackground(0);
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