#pragma once

#include "ofMain.h"
#include "ofxGui.h"

class Scanner
{

    Scanner();

public:
    static int x_pos;
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
};
