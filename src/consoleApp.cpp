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
    gui.add(gui_minLineLength.setup("min line length", 0, 0, 15));
    gui.add(gui_maxLineGap.setup("max line gap", 20, 0, 20));
    gui.add(gui_oscillationCenter.setup("oscillation center", IMAGE_HEIGHT / 2, 0, IMAGE_HEIGHT));
    gui.add(gui_upperRidgeLimit.setup("upper ridge limit", 0, 0, IMAGE_HEIGHT));
    gui.add(gui_lowerRidgeLimit.setup("lower ridge limit", IMAGE_HEIGHT, IMAGE_HEIGHT, 0));
    gui.add(gui_send_button.setup("scan and send compiled data", 8, 0, 15));
    gui.setPosition(0, 80);

    // communication with Pd
    Communication::sender.setup(HOST, SENDING_PORT);
    // send 1 to activate udp:
    ofxOscMessage m;
    m.setAddress("/setup/");
    m.addIntArg(1);
    Communication::sender.sendMessage(m, false);

    ofSleepMillis(1);

    Communication::receiver.setup(RECEIVING_PORT);
}

//--------------------------------------------------------------
void consoleApp::update()
{
    // update gui variables:
    Controls::img_threshold = gui_imgThreshold;  // img threshold
    Controls::canny_1 = gui_canny_1;             // img threshold
    Controls::canny_2 = gui_canny_2;             // img threshold
    Controls::edgeThreshold = gui_edgeThreshold; // line detection
    Controls::lineThreshold = gui_lineThreshold; // line detection
    Controls::minLineLength = gui_minLineLength; // line detection
    Controls::maxLineGap = gui_maxLineGap;       // line detection

    Scanner::oscillationCenter = gui_oscillationCenter; // upper scan area
    Scanner::upperRidgeLimit = gui_upperRidgeLimit;     // upper scan area
    // gui_upperRidgeLimit.setMax(gui_lowerRidgeLimit);
    Scanner::lowerRidgeLimit = gui_lowerRidgeLimit; // lower scan area
    // gui_lowerRidgeLimit.setMin(gui_upperRidgeLimit);

    static int prev_thresh = Controls::edgeThreshold;
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
                    m.setAddress("/line");
                    m.addIntArg(i);
                    m.addFloatArg(pt.x);
                    if (1 - pt.y / IMAGE_HEIGHT > 0.5)
                        m.addFloatArg(1 - (pt.y / IMAGE_HEIGHT));
                    Communication::sender.sendMessage(m, false);

                    ofSleepMillis(1);
                }
            }
        }
        gui_send_button = false;
    }

    ofxOscMessage incoming_message;
    Communication::receiver.getNextMessage(incoming_message);

    if (incoming_message.getAddress() == "/scanner/pos")
    {
        Scanner::x_pos = incoming_message.getArgAsInt(0);
        cout << "incoming message at " << incoming_message.getAddress() << ": " << incoming_message.getArgAsInt(0) << endl;
    }
}

//--------------------------------------------------------------
void consoleApp::draw()
{
    ofBackground(0);

    // display parameters:
    string c3Str = "canny_3 [c]: " + ofToString(Controls::canny_3);
    ofDrawBitmapString(c3Str, 20, 10);

    string draw_modes[5] = {"", "color", "grey", "sobel", "edge"};
    string drawStr = "draw_mode [1,2,3,4]: " + draw_modes[Controls::draw_mode];
    ofDrawBitmapString(drawStr, 20, 30);

    string fpsStr = "fps: " + ofToString(ofGetFrameRate());
    ofDrawBitmapString(fpsStr, 20, 50);

    string scannerPosStr = "scanner_pos: " + ofToString(Scanner::x_pos);
    ofDrawBitmapString(scannerPosStr, 20, 70);

    gui.draw();
}

//--------------------------------------------------------------
void consoleApp::keyPressed(int key)
{
}

//--------------------------------------------------------------
void consoleApp::keyReleased(int key)
{
    if (key == '1')
    {
        Controls::draw_mode = 1;
    }
    else if (key == '2')
    {
        Controls::draw_mode = 2;
    }
    else if (key == '3')
    {
        Controls::draw_mode = 3;
    }
    else if (key == '4')
    {
        Controls::draw_mode = 4;
    }
    else if (key == '0')
    {
        Controls::draw_mode = 0;
    }

    else if (key == 'c')
    {
        if (Controls::canny_3 == 3)
            Controls::canny_3 = 5;
        else if (Controls::canny_3 == 5)
            Controls::canny_3 = 7;
        else if (Controls::canny_3 == 7)
            Controls::canny_3 = 3;
    }

    else if (key == 'l')
        Scanner::do_draw_limits = !Scanner::do_draw_limits;

    else if (key == ' ')
    {
        Scanner::scanning = !Scanner::scanning;
    }
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