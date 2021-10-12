#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxGui.h"
#include "settings.h"
#include "ofxOsc.h"

using namespace cv;

//////////////////////////////// SCANNER //////////////////////////////
class Scanner
{

    Scanner();

public:
    static int scanned_pixels[IMAGE_WIDTH][IMAGE_HEIGHT];
    static int x_pos;

    static int upperRidgeLimit; // upper limit for white pixel detection
    static int lowerRidgeLimit; // lower limit for white pixel detection
    static bool do_draw_limits;

    static void draw();
    static void drawRidgeLimits();
    static void scan(ofPixels &pixels);
};

//////////////////////////// COMMUNICATION ////////////////////////////
class Communication
{
public:
    static ofxOscSender sender;
    static ofxOscReceiver receiver;
};

/////////////////////////////// CONTROLS //////////////////////////////
class Controls
{

public:
    // which image to draw
    static int draw_mode;

    // image threshold
    static int img_threshold;

    // Hough Transform Lines
    static bool do_edgeDetection;
    static int edgeThreshold;
    static int lineThreshold;
    static int lowThreshold;
    static int minLineLength;
    static int maxLineGap;
    static int rho;

    // Canny Thresholds
    static int canny_1;
    static int canny_2;
    static int canny_3;

    static vector<Vec4i> lines; // from ofxOpenCv
};
