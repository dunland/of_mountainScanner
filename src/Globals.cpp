#include "Globals.h"
#include "ofMain.h"

//////////////////////////////// GLOBALS //////////////////////////////

// images:
string Globals::images[3] = {"mountain_1920x1080.JPG", "02.JPG", "03.JPG"};
int Globals::img_idx = 0;
ofxCvColorImage Globals::colorImg;
ofxCvGrayscaleImage Globals::grayImg;

// edge detection:
ofImage Globals::img, Globals::edge_img, Globals::sobel_img;

//////////////////////////////// SCANNER //////////////////////////////
Scan_Mode Scanner::scan_mode = Relative;
bool Scanner::scanning = false;
int Scanner::scan_iteration = 0;
int Scanner::maxIterations = 2;
int Scanner::whitePixelsAbsolute[IMAGE_WIDTH];

int Scanner::x_pos = 0;
int Scanner::ymin = IMAGE_HEIGHT;
int Scanner::ymax = 0;

int Scanner::oscillationCenter = IMAGE_HEIGHT / 2;
int Scanner::upperRidgeLimit = IMAGE_HEIGHT / 3;
int Scanner::lowerRidgeLimit = IMAGE_HEIGHT * 3 / 4;
bool Scanner::do_draw_limits = true;

// draw vertical rectangle at scanner position
// TODO: inverted color bar of original image; using shader?
void Scanner::draw()
{
    ofSetColor(250);
    ofFill();
    ofSetLineWidth(1);
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

    // rectangle at scanning point:
    ofSetColor(250, 20, 20);
    ofNoFill();
    ofSetLineWidth(3);
    ofDrawRectangle(ofRectangle(x_pos - 5, whitePixelsAbsolute[x_pos] - 5, 10, 10));
}

// draw horizontal lines as limiters for white pixel detection:
void Scanner::drawRidgeLimits()
{
    ofSetColor(250);
    ofSetLineWidth(1);

    ofDrawLine(0, upperRidgeLimit, ofGetWidth(), upperRidgeLimit);
    ofDrawLine(0, lowerRidgeLimit, ofGetWidth(), lowerRidgeLimit);
    ofSetColor(70, 20, 200);
    ofDrawLine(0, oscillationCenter, ofGetWidth(), oscillationCenter);
}

// just returns min and max values of white pixels within limits
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
            scan_iteration++;

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
        float y_out = oscillationCenter; // start at osc center --> will be default if no pixels found
        for (int y = Scanner::upperRidgeLimit; y < Scanner::lowerRidgeLimit; y++)
        {
            if (pixels.getColor(x_pos, y) == ofColor(255, 255, 255))
            {
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
            scan_iteration++;

        previous_x_pos = x_pos;
    }
}

void Scanner::quickScan_relative(ofPixels &pixels)
{
    ofxOscMessage m;
    m.setAddress("/quickScan/" + ofToString(Globals::img_idx));

    getMinMax(pixels);

    for (int x = 0; x < IMAGE_WIDTH; x++)
        for (int y = 0; y < IMAGE_HEIGHT; y++)
            whitePixelsAbsolute[x] = oscillationCenter;

    for (int x = 0; x < IMAGE_WIDTH; x++)
    {
        float y_out = oscillationCenter; // start at osc center --> will be default if no pixels found
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
    Communication::sender.sendMessage(m, false);
    cout << "sent quickscan data: " << m << endl;
}

//////////////////////////// COMMUNICATION ////////////////////////////
ofxOscSender Communication::sender;
ofxOscReceiver Communication::receiver;

/////////////////////////////// CONTROLS //////////////////////////////
int Controls::draw_mode = 4;
int Controls::img_thresholdLow;
int Controls::img_thresholdHigh;

// Hough Lines
bool Controls::do_edgeDetection = false;
bool Controls::doQuickScanNextUpdate = false;
bool Controls::doLoadNextImage = false;
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

void Controls::loadNextImage()
{
    // images setup:
    Globals::img_idx = (Globals::img_idx + 1) % NUM_OF_IMAGES;

    Globals::img.load(Globals::images[Globals::img_idx]);
    Globals::img.resize(IMAGE_WIDTH, IMAGE_HEIGHT);

    Globals::colorImg.allocate(Globals::img.getWidth(), Globals::img.getHeight());
    Globals::grayImg.allocate(Globals::img.getWidth(), Globals::img.getHeight());

    Globals::colorImg.setFromPixels(Globals::img.getPixels());
    Globals::colorImg.convertToGrayscalePlanarImage(Globals::grayImg, 0);
    Globals::grayImg.threshold(Controls::img_thresholdLow, Controls::img_thresholdHigh);

    // edge detection:
    Canny(Globals::grayImg, Globals::edge_img, Controls::canny_1, Controls::canny_2, Controls::canny_3);
    Sobel(Globals::grayImg, Globals::sobel_img);
    Globals::sobel_img.update();
    Globals::edge_img.update();

    cout << "loading image " + Globals::images[Globals::img_idx] << endl;

    Scanner::scan_iteration = 0;
    Controls::doQuickScanNextUpdate = true;
}