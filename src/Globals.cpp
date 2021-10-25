#include "Globals.h"
#include "ofMain.h"

//////////////////////////////// GLOBALS //////////////////////////////

// images:
vector<string> Globals::images;
int Globals::img_idx = 0;
ofxCvColorImage Globals::colorImg;
ofxCvGrayscaleImage Globals::grayImg;

// edge detection:
ofImage Globals::img, Globals::scaledImage, Globals::edge_img, Globals::scaledEdgeImage, Globals::sobel_img;

//////////////////////////////// SCANNER //////////////////////////////
Scan_Mode Scanner::scan_mode = Relative;
bool Scanner::scanning = false;
int Scanner::scan_iteration = 0;
int Scanner::maxIterations = MAX_SCAN_ITERATIONS;
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
    float n = IMAGE_SCALING;

    ofSetColor(250);
    ofFill();
    ofSetLineWidth(1);
    switch (Scanner::scan_mode)
    {
    case Absolute:
        ofDrawRectangle(ofRectangle(x_pos * n, 0, 3, ofGetWindowHeight() * n));
        // draw twice if scaling mode == 0.5:
        if (IMAGE_SCALING == 0.5)
        {
            ofPushMatrix();
            ofTranslate(0, IMAGE_HEIGHT * IMAGE_SCALING);
            ofDrawRectangle(ofRectangle(x_pos * n, 0, 3, ofGetWindowHeight() * n));
            ofPopMatrix();
        }
        break;

    case Relative:
        ofDrawRectangle(ofRectangle(x_pos * n, upperRidgeLimit * n, 3, lowerRidgeLimit * n - upperRidgeLimit * n));
        // draw twice if scaling mode == 0.5:
        if (IMAGE_SCALING == 0.5)
        {
            ofPushMatrix();
            ofTranslate(0, IMAGE_HEIGHT * IMAGE_SCALING);
            ofDrawRectangle(ofRectangle(x_pos * n, upperRidgeLimit * n, 3, lowerRidgeLimit * n - upperRidgeLimit * n));
            ofPopMatrix();
        }
        break;

    default:
        break;
    }

    // rectangle at scanning point:
    ofSetColor(250, 20, 20);
    ofNoFill();
    ofSetLineWidth(3);
    ofDrawRectangle(ofRectangle(x_pos * n - 5, int(whitePixelsAbsolute[x_pos] * n) - 5, 10, 10));

    // draw twice if scaling mode == 0.5:
    if (IMAGE_SCALING == 0.5)
    {
        ofPushMatrix();
        ofTranslate(0, IMAGE_HEIGHT * IMAGE_SCALING);
        ofSetColor(250, 20, 20);
        ofNoFill();
        ofSetLineWidth(3);
        ofDrawRectangle(ofRectangle(x_pos * n - 5, int(whitePixelsAbsolute[x_pos] * n) - 5, 10, 10));
        ofPopMatrix();
    }
}

// draw horizontal lines as limiters for white pixel detection:
void Scanner::drawRidgeLimits()
{
    float n = IMAGE_SCALING;

    ofSetLineWidth(1);

    // ridge limits (white)
    ofSetColor(250);
    ofDrawLine(0, upperRidgeLimit * n, ofGetWidth(), upperRidgeLimit * n);
    ofDrawLine(0, lowerRidgeLimit * n, ofGetWidth(), lowerRidgeLimit * n);

    // oscillation center (blue)
    ofSetColor(70, 20, 200);
    ofDrawLine(0, oscillationCenter * n, ofGetWidth(), oscillationCenter * n);

    // draw twice if scaling mode = 0.5
    if (IMAGE_SCALING == 0.5)
    {
        ofPushMatrix();
        ofTranslate(0, IMAGE_HEIGHT * IMAGE_SCALING);
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
    for (int i = 0; i < SCANNING_SPEED; i++) // num of scans per frame
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
    for (int i = 0; i < SCANNING_SPEED; i++) // num of scans per frame
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
void Scanner::quickScan_relative(ofPixels &pixels)
{
    ofxOscMessage m;
    m.setAddress("/quickScan/" + ofToString(Globals::img_idx % PD_NUM_OF_CHARTS));

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
    Globals::img_idx = (Globals::img_idx + 1) % Globals::images.size();

    Globals::img.load(Globals::images.at(Globals::img_idx));
    Globals::img.resize(IMAGE_WIDTH, IMAGE_HEIGHT); // ATTENTION: IMAGE_WIDTH AND IMAGE_HEIGHT DO AFFECT THE SCANNING. TODO: RESIZE ONLY SHORTLY BEFORE DRAWING (USING ANOTHER INSTANCE)

    Globals::scaledImage.loadImage(Globals::images.at(Globals::img_idx));
    Globals::scaledImage.resize(IMAGE_WIDTH * IMAGE_SCALING, IMAGE_HEIGHT * IMAGE_SCALING);

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

    ofLogNotice("loading image " + Globals::images.at(Globals::img_idx));

    Scanner::scan_iteration = 0;
    Controls::doQuickScanNextUpdate = true;
}