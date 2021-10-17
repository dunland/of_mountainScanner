#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxGui.h"
#include "settings.h"
#include "ofxOsc.h"

using namespace cv;

enum Scan_Mode
{
    Absolute = 0,
    Relative = 1
};

class Globals
{
    public:
    // images:
    static string images[3];
    static int img_idx;
    static ofxCvColorImage colorImg;
    static ofxCvGrayscaleImage grayImg;

    // edge detection:
    static ofImage img, gray_img, edge_img, sobel_img;
};

//////////////////////////////// SCANNER //////////////////////////////
class Scanner
{

    Scanner();

public:
    static Scan_Mode scan_mode;
    static bool scanning;
    static int scan_iteration;
    static int maxIterations;

    static int x_pos;
    static int ymin;
    static int ymax;

    static int oscillationCenter;
    static int upperRidgeLimit; // upper limit for white pixel detection
    static int lowerRidgeLimit; // lower limit for white pixel detection
    static bool do_draw_limits;

    static void draw();
    static void drawRidgeLimits();
    static void getMinMax(ofPixels &pixels);     // performs an initial quick scan of the outline
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
