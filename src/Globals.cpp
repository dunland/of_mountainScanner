#include "Globals.h"
#include "ofMain.h"

//////////////////////////////// SCANNER //////////////////////////////
int Scanner::x_pos = 0;
int Scanner::upperRidgeLimit = 0;
int Scanner::lowerRidgeLimit = IMAGE_HEIGHT;
bool Scanner::do_draw_limits = true;

// draw vertical rectangle at scanner position
void Scanner::draw()
{
    ofSetColor(255);
    ofRectangle rect_(x_pos, 0, 3, 1080);
    ofDrawRectangle(rect_);
}

// draw horizontal lines as limiters for white pixel detection:
void Scanner::drawRidgeLimits()
{
    ofSetColor(255);
    ofDrawLine(0, upperRidgeLimit, ofGetWidth(), upperRidgeLimit);
    ofDrawLine(0, lowerRidgeLimit, ofGetWidth(), lowerRidgeLimit);
}

// look for white pixels in vertical bar:
void Scanner::scan(ofPixels &pixels)
{
    x_pos = (x_pos + 1) % IMAGE_WIDTH;

    static int previous_x_pos = 0;
    if (x_pos != previous_x_pos)
    {
        for (int y = 0; y < IMAGE_HEIGHT; y++)
        {
            if (pixels.getColor(x_pos, y) == ofColor(255, 255, 255) && y >= Scanner::upperRidgeLimit && y <= Scanner::lowerRidgeLimit)
            {
                cout << "white pixel at: (" << x_pos << "|" << y << ")" << endl;

                // send data:
                ofxOscMessage m;
                m.setAddress("/rt_scan");
                m.addFloatArg(x_pos);
                m.addFloatArg(y);
                Communication::sender.sendMessage(m, false);
            }
        }
        previous_x_pos = x_pos;
    }
}

//////////////////////////// COMMUNICATION ////////////////////////////
ofxOscSender Communication::sender;
ofxOscReceiver Communication::receiver;

/////////////////////////////// CONTROLS //////////////////////////////
int Controls::draw_mode = 3;
int Controls::img_threshold;

// Hough Lines
bool Controls::do_edgeDetection = false;
int Controls::edgeThreshold;
int Controls::lineThreshold;
int Controls::lowThreshold;
int Controls::minLineLength = 15;
int Controls::maxLineGap = 20;

// Canny Thresholds:
int Controls::canny_1;
int Controls::canny_2;
int Controls::canny_3 = 3;

vector<Vec4i> Controls::lines;