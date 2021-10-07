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
    gui.add(gui_lineThreshold.setup("Line Threshold", 0, 0, 200));
    gui.add(gui_send_button.setup("send osc data", 0, 15, 15));

    // communication with Pd
    sender.setup(HOST, PORT);
}

//--------------------------------------------------------------
void consoleApp::update()
{
    // update gui variables:
    Controls::img_threshold = gui_imgThreshold;  // img threshold
    Controls::edgeThreshold = gui_edgeThreshold; // line detection
    Controls::lineThreshold = gui_lineThreshold; // line detection
    Controls::lowThreshold = gui_lowThreshold;   // line detection

    static int prev_thresh = Controls::edgeThreshold; // TODO: send message with every gui update
    if (gui_send_button)
    {

        for (int i = 0; i < Controls::lines.size(); i++)
        {
            float x1 = Controls::lines[i][0];
            float y1 = Controls::lines[i][1];
            float x2 = Controls::lines[i][2];
            float y2 = Controls::lines[i][3];
            ofPolyline l;
            l.addVertex(x1, y1);
            l.addVertex(x2, y2);

            for (float j = 0; j < 1; j += 0.01) // iterating the line
            {
                ofPoint pt = l.getPointAtPercent(j);
                static ofPoint pt_previous = pt;

                // get one y for each int(x):
                if (int(pt.x) != int(pt_previous.x))
                {
                    cout << i << " " << pt.x << " " << pt.y << endl;
                    pt_previous = pt;

                    // send data:
                    ofxOscMessage m;
                    m.setAddress("/line/" + ofToString(i));
                    m.addFloatArg(pt.x);
                    m.addFloatArg(1 - (pt.y / IMAGE_HEIGHT));
                    sender.sendMessage(m, false);

                    ofSleepMillis(1);
                }
            }
        }
        gui_send_button = false;
    }
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