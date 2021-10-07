#include "Globals.h"
#include "ofMain.h"

int Scanner::x_pos = 0;
void Scanner::draw()
{
    ofSetColor(255);
    ofRectangle rect_(x_pos, 0, 5, 1080);
    ofDrawRectangle(rect_);
}

int Controls::img_threshold;

// Hough Lines
int Controls::edgeThreshold;
int Controls::lineThreshold;
int Controls::lowThreshold;

// Canny Thresholds:
int Controls::canny_1;
int Controls::canny_2;

vector<Vec4i> Controls::lines;