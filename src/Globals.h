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
    // gui:
    static int edgeThreshold;

    //    Hough Transform Lines
    static int lineThreshold;
    static int lowThreshold;
};
