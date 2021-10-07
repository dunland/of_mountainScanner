#include "Globals.h"

int Scanner::x_pos = 0;

int Controls::img_threshold;

// Hough Lines
int Controls::edgeThreshold;
int Controls::lineThreshold;
int Controls::lowThreshold;

// Canny Thresholds:
int Controls::canny_1;
int Controls::canny_2;

vector<Vec4i> Controls::lines;