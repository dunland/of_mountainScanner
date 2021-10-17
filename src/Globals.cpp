#include "Globals.h"
#include "ofMain.h"

//////////////////////////////// SCANNER //////////////////////////////
Scan_Mode Scanner::scan_mode = Absolute;
bool Scanner::scanning = false;
int Scanner::scan_iteration = 0;
int Scanner::maxIterations = 2;

int Scanner::x_pos = 0;
int Scanner::ymin = IMAGE_HEIGHT;
int Scanner::ymax = 0;

int Scanner::oscillationCenter = IMAGE_HEIGHT / 2;
int Scanner::upperRidgeLimit = 0;
int Scanner::lowerRidgeLimit = IMAGE_HEIGHT;
bool Scanner::do_draw_limits = true;

// draw vertical rectangle at scanner position
// TODO: inverted color bar of original image; using shader?
void Scanner::draw()
{
    ofSetColor(250);
    switch (Scanner::scan_mode)
    {
    case Absolute:
        ofDrawRectangle(ofRectangle(x_pos, 0, 3, 1080));
        break;

    case Relative:
        ofDrawRectangle(ofRectangle(x_pos, upperRidgeLimit, 3, lowerRidgeLimit - upperRidgeLimit));
        break;

    default:
        break;
    }
}

// draw horizontal lines as limiters for white pixel detection:
void Scanner::drawRidgeLimits()
{
    ofSetColor(250);
    ofDrawLine(0, upperRidgeLimit, ofGetWidth(), upperRidgeLimit);
    ofDrawLine(0, lowerRidgeLimit, ofGetWidth(), lowerRidgeLimit);
    ofDrawLine(0, oscillationCenter, ofGetWidth(), oscillationCenter);
}

// perform an initial quick scan of the outline
void Scanner::getMinMax(ofPixels &pixels)
{
    ymin = IMAGE_HEIGHT;
    ymax = 0;
    // get ymin and ymax for each x:
    for (int x = 0; x < IMAGE_WIDTH; x++)
    {
        for (int y = 0; y < IMAGE_HEIGHT; y++)
        {
            if (pixels.getColor(x, y) == ofColor(255, 255, 255))
            {
                switch (Scanner::scan_mode)
                {
                case Relative:
                    if (y >= upperRidgeLimit && y <= lowerRidgeLimit)
                    {
                        if (y < ymin)
                            ymin = y;
                        if (y > ymax)
                            ymax = y;
                    }
                    break;

                case Absolute:
                    if (y < ymin)
                        ymin = y;
                    if (y > ymax)
                        ymax = y;
                    break;

                default:
                    break;
                }
            }
        }
    }
}

// look for white pixels within limits in vertical bar:
void Scanner::scan_absolute(ofPixels &pixels)
{
    x_pos = (x_pos + 1) % IMAGE_WIDTH;

    static int previous_x_pos = 0;
    if (x_pos != previous_x_pos)
    {
        for (int y = 0; y < IMAGE_HEIGHT; y++)
        {
            if (pixels.getColor(x_pos, y) == ofColor(255, 255, 255) && y >= Scanner::upperRidgeLimit && y <= Scanner::lowerRidgeLimit)
            {
                float y_out = 1 - (y / 1080.0);
                // cout << "white pixel at: (" << x_pos << "|" << y_out << ")" << endl;

                // send data:
                ofxOscMessage m;
                m.setAddress("/rt_scan");
                m.addFloatArg(x_pos);
                m.addFloatArg(y_out);
                Communication::sender.sendMessage(m, false);
            }
        }

        if (x_pos >= IMAGE_WIDTH - 1)
        {
            scan_iteration++;
            cout << "reached end of image. scan_iteration ++" << endl;
        }

        previous_x_pos = x_pos;
    }
}

// converts white pixels within limits relative to oscillationCenter
void Scanner::scan_relative(ofPixels &pixels)
{
    x_pos = (x_pos + 1) % IMAGE_WIDTH;

    static int previous_x_pos = 0;
    if (x_pos != previous_x_pos)
    {
        for (int y = 0; y < IMAGE_HEIGHT; y++)
        {
            if (pixels.getColor(x_pos, y) == ofColor(255, 255, 255) && y >= Scanner::upperRidgeLimit && y <= Scanner::lowerRidgeLimit)
            {
                float y_out;

                if (y < Scanner::oscillationCenter) // positive values above oscLine
                {
                    y_out = (Scanner::oscillationCenter - y) / float(Scanner::oscillationCenter - Scanner::ymin);
                }
                else if (y > Scanner::oscillationCenter) // negative values below oscLine
                {
                    y_out = (y - Scanner::oscillationCenter) / float(Scanner::ymax - Scanner::oscillationCenter) * -1;
                }

                // cout << "white pixel at: (" << x_pos << "|" << y_out << ")" << endl;

                // send data:
                ofxOscMessage m;
                m.setAddress("/rt_scan");
                m.addFloatArg(x_pos);
                m.addFloatArg(y_out);
                Communication::sender.sendMessage(m, false);
            }
        }

        if (x_pos >= IMAGE_WIDTH - 1)
        {
            scan_iteration++;
            cout << "reached end of image. scan_iteration ++" << endl;
        }

        previous_x_pos = x_pos;
    }
}

//////////////////////////// COMMUNICATION ////////////////////////////
ofxOscSender Communication::sender;
ofxOscReceiver Communication::receiver;

/////////////////////////////// CONTROLS //////////////////////////////
int Controls::draw_mode = 4;
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