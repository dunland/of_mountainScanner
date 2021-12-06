#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxCv.h"
#include "ofxGui.h"
#include "settings.h"
#include "ofxOsc.h"
#include "Circles.h"

using namespace cv;
using namespace ofxCv;

enum Scan_Mode
{
    Absolute = 0,
    Relative = 1
};

enum MinMaxMode
{
    Limits = 0,    // upper and lower ridge limits are min and max; no normalization
    Normalized = 1 // get highest and lowest mountain peaks within limits
};

class Globals
{
public:
    // images:
    static vector<string> images;
    static int img_idx;
    static float image_scaling;
    static ofxCvColorImage colorImg;
    static ofxCvGrayscaleImage grayImg;
    static ofImage img, scaledImage;

    // edge detection:
    static ofImage edge_img, scaledEdgeImage, sobel_img;

    // circles:
    static vector<Circle*> circles;
};

//////////////////////////////// SCANNER //////////////////////////////
class Scanner
{

    Scanner();

public:
    static bool scanning;
    static int whitePixelsAbsolute[IMAGE_WIDTH];

    static int x_pos; // current postion of the scanner
    static int ymin;  // uppermost white pixel within boundaries
    static int ymax;  // lowest white pixel within boundaries

    static int oscillationCenter;
    static int upperRidgeLimit; // upper limit for white pixel detection
    static int lowerRidgeLimit; // lower limit for white pixel detection
    static bool do_draw_limits;

    // behavior:
    static Scan_Mode scan_mode;
    static MinMaxMode min_max_mode;
    static int scan_iteration;
    static int maxIterations;
    static int scanning_speed;

    static void draw();
    static void drawRidgeLimits();               // draws limits for pixel detection and oscillation center line
    static void getMinMax(ofPixels &pixels);     // calculates min and max y values within the boundaries
    static void scan_absolute(ofPixels &pixels); // looking for white pixels within limits
    static void scan_relative(ofPixels &pixels); // looking for white pixels within limits and convert relative to oscillationCenter
    static void quickScan_relative(ofPixels &pixels);
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
    static int img_thresholdLow;
    static int img_thresholdHigh;

    // Hough Transform Lines
    static bool do_edgeDetection;
    static bool doQuickScanNextUpdate;
    static bool doLoadNextImage; // flag to load next image during next update
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

    static void loadNextImage();
};
