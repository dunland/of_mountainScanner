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

class Globals
{
public:
    // images:
    static vector<string> imagePaths;
    static int img_idx;
    static float image_scaling;
    static ofxCvColorImage colorImg;
    static ofxCvGrayscaleImage grayImg;
    static ofImage img, nextImg, scaledImage;

    // edge detection:
    static ofImage edge_img, scaledEdgeImage, sobel_img;

    // circles:
    static vector<Circle *> circles;
    static float circleShrinkSpeed;
    static int circlesCreationStep;
    static bool circlesFlyOff;
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
