#include "Scanner.h"
#include "Globals.h"
#include "ofMain.h"

//////////////////////////////// SCANNER //////////////////////////////
bool Scanner::scanning = false;
int Scanner::whitePixelsAbsolute[IMAGE_WIDTH];

int Scanner::x_pos = 0;
int Scanner::ymin = IMAGE_HEIGHT;
int Scanner::ymax = 0;

int Scanner::oscillationCenter = IMAGE_HEIGHT / 2;
int Scanner::upperRidgeLimit = IMAGE_HEIGHT / 3;
int Scanner::lowerRidgeLimit = IMAGE_HEIGHT * 3 / 4;
bool Scanner::do_draw_limits = true;
bool Scanner::do_draw = true;

// behavior:
Scan_Mode Scanner::scan_mode = Relative;
MinMaxMode Scanner::min_max_mode = Limits;
int Scanner::scan_iteration = 0;
int Scanner::maxIterations = MAX_SCAN_ITERATIONS;
int Scanner::scanning_speed = SCANNING_SPEED;

// draw vertical rectangle at scanner position
// TODO: inverted color bar of original image; using shader?
void Scanner::draw()
{
    float n = Globals::image_scaling;

    ofSetColor(250);
    ofFill();
    ofSetLineWidth(1);
    if (do_draw)
    {
        switch (Scanner::scan_mode)
        {
        case Absolute:
            ofDrawRectangle(ofRectangle(x_pos * n, 0, 3, ofGetWindowHeight() * n));
            if (Globals::image_scaling == 0.5)
            {
                ofPushMatrix();
                ofTranslate(0, IMAGE_HEIGHT * Globals::image_scaling);
                ofDrawRectangle(ofRectangle(x_pos * n, 0, 3, ofGetWindowHeight() * n));
                ofPopMatrix();
            }
            break;

        case Relative:
            ofDrawRectangle(ofRectangle(x_pos * n, upperRidgeLimit * n, 3, lowerRidgeLimit * n - upperRidgeLimit * n));
            if (Globals::image_scaling == 0.5)
            {
                ofPushMatrix();
                ofTranslate(0, IMAGE_HEIGHT * Globals::image_scaling);
                ofDrawRectangle(ofRectangle(x_pos * n, upperRidgeLimit * n, 3, lowerRidgeLimit * n - upperRidgeLimit * n));
                ofPopMatrix();
            }
            break;

        default:
            break;
        }
    }

    // rectangle at scanning point:
    ofSetColor(250, 20, 20);
    ofNoFill();
    ofSetLineWidth(3);
    ofDrawRectangle(ofRectangle(x_pos * n - 5, int(whitePixelsAbsolute[x_pos] * n) - 5, 10, 10));

    // circle creation:
    if (x_pos % Globals::circlesCreationStep == 0 && scanning)
        Globals::circles.push_back(new Circle(x_pos, int(whitePixelsAbsolute[x_pos]), Globals::circlesCreationStep));

    // draw twice if scaling mode == 0.5:
    if (Globals::image_scaling == 0.5 && scanning)
    {
        ofPushMatrix();
        ofTranslate(0, IMAGE_HEIGHT * Globals::image_scaling);
        ofSetColor(250, 20, 20);
        ofNoFill();
        ofSetLineWidth(3);
        ofDrawRectangle(ofRectangle(x_pos * n - 5, int(whitePixelsAbsolute[x_pos] * n) - 5, 10, 10));
        ofPopMatrix();

        // circle creation:
        if (x_pos % Globals::circlesCreationStep == 0)
            Globals::circles.push_back(new Circle(x_pos, int(whitePixelsAbsolute[x_pos]), Globals::circlesCreationStep));
    }
}

// draw horizontal lines as limiters for white pixel detection:
void Scanner::drawRidgeLimits()
{
    float n = Globals::image_scaling;

    ofSetLineWidth(1);

    // ridge limits (white)
    ofSetColor(250);
    ofDrawLine(0, upperRidgeLimit * n, ofGetWidth(), upperRidgeLimit * n);
    ofDrawLine(0, lowerRidgeLimit * n, ofGetWidth(), lowerRidgeLimit * n);

    // oscillation center (blue)
    ofSetColor(70, 20, 200);
    ofDrawLine(0, oscillationCenter * n, ofGetWidth(), oscillationCenter * n);

    // draw twice if scaling mode = 0.5
    if (Globals::image_scaling == 0.5)
    {
        ofPushMatrix();
        ofTranslate(0, IMAGE_HEIGHT * Globals::image_scaling);
        // ridge limits (white)
        ofSetColor(250);
        ofDrawLine(0, upperRidgeLimit * n, ofGetWidth(), upperRidgeLimit * n);
        ofDrawLine(0, lowerRidgeLimit * n, ofGetWidth(), lowerRidgeLimit * n);

        // oscillation center (blue)
        ofSetColor(70, 20, 200);
        ofDrawLine(0, oscillationCenter * n, ofGetWidth(), oscillationCenter * n);
        ofPopMatrix();
    }
}

// just returns min and max values of white pixels within limits
void Scanner::getMinMax(ofPixels &pixels)
{
    ymin = IMAGE_HEIGHT;
    ymax = 0;

    switch (min_max_mode)
    {
    case Normalized:
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
        break;

    case Limits:
        ymin = upperRidgeLimit;
        ymax = lowerRidgeLimit;

    default:
        break;
    }
}

// look for white pixels within limits in vertical bar:
void Scanner::scan_absolute(ofPixels &pixels)
{
    for (int i = 0; i < Scanner::scanning_speed; i++) // num of scans per frame
    {
        x_pos = (x_pos + 1) % IMAGE_WIDTH;

        static int previous_x_pos = 0;
        if (x_pos != previous_x_pos)
        {
            float y_out = oscillationCenter; // start at osc center --> will be default if no pixels found
            for (int y = 0; y < IMAGE_HEIGHT; y++)
            {
                if (pixels.getColor(x_pos, y) == ofColor(255, 255, 255))
                {
                    y_out = 1 - (y / 1080.0);
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
                if (Scanner::scan_iteration >= Scanner::maxIterations)
                    Controls::loadNextImage();
            }

            previous_x_pos = x_pos;
        }
    }
}

// converts white pixels within limits relative to oscillationCenter
void Scanner::scan_relative(ofPixels &pixels)
{
    for (int i = 0; i < Scanner::scanning_speed; i++) // num of scans per frame
    {
        x_pos = (x_pos + 1) % IMAGE_WIDTH;

        static int previous_x_pos = 0;
        if (x_pos != previous_x_pos)
        {
            float y_out = 0;
            for (int y = upperRidgeLimit; y < lowerRidgeLimit; y++)
            {
                if (pixels.getColor(x_pos, y) == ofColor(255, 255, 255))
                {
                    if (y < oscillationCenter) // positive values above oscLine
                    {
                        y_out = (Scanner::oscillationCenter - y) / float(Scanner::oscillationCenter - Scanner::ymin);
                    }
                    else if (y > oscillationCenter) // negative values below oscLine
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

                    // ofLogNotice(ofToString(Scanner::x_pos) + " " + ofToString(Scanner::whitePixelsAbsolute[Scanner::x_pos]) + " " + ofToString(Scanner::ymin) + " " + ofToString(Scanner::oscillationCenter) + " " + ofToString(Scanner::ymax) + " " + ofToString(y_out));
                }
            }

            if (x_pos >= IMAGE_WIDTH - 1)
            {
                scan_iteration++;
                if (Scanner::scan_iteration >= Scanner::maxIterations)
                    Controls::loadNextImage();
            }

            previous_x_pos = x_pos;
        }
    }
}

// -------------------------- QUICK SCANNING --------------------------
// prepend image index as int, don't use it as message address
void Scanner::quickScan_relative(ofPixels &pixels)
{
    ofxOscMessage m;
    m.setAddress("/quickScan");
    m.addIntArg(Globals::img_idx % PD_NUM_OF_CHARTS); // send int as index first

    getMinMax(pixels);

    for (int x = 0; x < IMAGE_WIDTH; x++)
        for (int y = 0; y < IMAGE_HEIGHT; y++)
            whitePixelsAbsolute[x] = oscillationCenter;

    for (int x = 0; x < IMAGE_WIDTH; x++)
    {
        float y_out = 0;
        for (int y = upperRidgeLimit; y < lowerRidgeLimit; y++)
        {
            if (pixels.getColor(x, y) == ofColor(255, 255, 255))
            {

                if (y < oscillationCenter) // positive values above oscLine
                {
                    y_out = (oscillationCenter - y) / float(oscillationCenter - ymin);
                }
                else if (y > oscillationCenter) // negative values below oscLine
                {
                    y_out = (y - oscillationCenter) / float(ymax - oscillationCenter) * -1;
                }

                m.addFloatArg(y_out);
                whitePixelsAbsolute[x] = y;
                break; // if one y was found, go to next x // TODO: think about something better!
            }
        }
    }

    // send data:
    Communication::sender.sendMessage(m, false); // sends space-separated list of floats
    ofLogNotice("sent quickscan data: " + ofToString(m));
}

// scanning without prepending index:
// void Scanner::quickScan_relative(ofPixels &pixels)
// {
//     ofxOscMessage m;
//     m.setAddress("/quickScan/" + ofToString(Globals::img_idx % PD_NUM_OF_CHARTS));

//     getMinMax(pixels);

//     for (int x = 0; x < IMAGE_WIDTH; x++)
//         for (int y = 0; y < IMAGE_HEIGHT; y++)
//             whitePixelsAbsolute[x] = oscillationCenter;

//     for (int x = 0; x < IMAGE_WIDTH; x++)
//     {
//         float y_out = 0;
//         for (int y = upperRidgeLimit; y < lowerRidgeLimit; y++)
//         {
//             if (pixels.getColor(x, y) == ofColor(255, 255, 255))
//             {

//                 if (y < oscillationCenter) // positive values above oscLine
//                 {
//                     y_out = (oscillationCenter - y) / float(oscillationCenter - ymin);
//                 }
//                 else if (y > oscillationCenter) // negative values below oscLine
//                 {
//                     y_out = (y - oscillationCenter) / float(ymax - oscillationCenter) * -1;
//                 }

//                 m.addFloatArg(y_out);
//                 whitePixelsAbsolute[x] = y;
//                 break; // if one y was found, go to next x // TODO: think about something better!
//             }
//         }
//     }

//     // send data:
//     Communication::sender.sendMessage(m, false); // sends space-separated list of floats
//     ofLogNotice("sent quickscan data: " + ofToString(m));
// }