#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxGui.h"

using namespace cv;

class Scanner
{

    Scanner();

public:
    static int x_pos;

    static void draw();
};

class Controls
{

public:

    // image threshold
    static int img_threshold;

    // Hough Transform Lines
    static int edgeThreshold;
    static int lineThreshold;
    static int lowThreshold;

    // Canny Thresholds
    static int canny_1;
    static int canny_2;

    static vector<Vec4i> lines; // from ofxOpenCv
};
