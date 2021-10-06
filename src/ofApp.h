#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "scanner.h"
#include "ofxCv.h"
#include "ofxGui.h"

using namespace cv;
using namespace ofxCv;

class ofApp : public ofBaseApp
{
public:
	void setup();
	void update();
	void draw();

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);

	// gui:
	ofxPanel gui;
    ofxIntSlider edgeThreshold;

    //    Hough Transform Lines
    ofxIntSlider lineThreshold;
    ofxIntSlider lowThreshold;

	// images:
	ofxCvColorImage colorImg;
	ofxCvGrayscaleImage grayImg;

	// edge detection:
	ofImage img, gray_img, edge_img, sobel_img;

	// line detection:
	Mat imgMat;
	Mat imgMatGray;
	Mat img_edgeMat;
	Mat img_frameMat;

	int mouse_x;
};