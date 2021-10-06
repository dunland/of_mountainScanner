#pragma once
#include "ofxGui.h"
#include "ofMain.h"

class Controls
{

public:
	// gui:
    static ofxIntSlider edgeThreshold;

    //    Hough Transform Lines
    static ofxIntSlider lineThreshold;
    static ofxIntSlider lowThreshold;
};