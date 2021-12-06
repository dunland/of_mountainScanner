#include "Globals.h"
#include "ofMain.h"
#include "settings.h"

//////////////////////////////// GLOBALS //////////////////////////////

// images:
vector<string> Globals::imagePaths;
int Globals::img_idx = 0;
float Globals::image_scaling = IMAGE_SCALING;
ofxCvColorImage Globals::colorImg;
ofxCvGrayscaleImage Globals::grayImg;

// edge detection:
ofImage Globals::img, Globals::nextImg, Globals::scaledImage, Globals::edge_img, Globals::scaledEdgeImage, Globals::sobel_img;

// circles:
vector<Circle *> Globals::circles;
float Globals::circleShrinkSpeed = CIRCLES_SHRINK_SPEED;
int Globals::circlesCreationStep = CIRCLES_CREATION_STEP;
bool Globals::circlesFlyOff = false;

//////////////////////////// COMMUNICATION ////////////////////////////
ofxOscSender Communication::sender;
ofxOscReceiver Communication::receiver;

/////////////////////////////// CONTROLS //////////////////////////////
int Controls::draw_mode = 4;
int Controls::img_thresholdLow;
int Controls::img_thresholdHigh;

// Hough Lines
bool Controls::do_edgeDetection = false;
bool Controls::doQuickScanNextUpdate = false;
bool Controls::doLoadNextImage = false;
int Controls::edgeThreshold;
int Controls::lineThreshold;
int Controls::lowThreshold;
int Controls::minLineLength = 15;
int Controls::maxLineGap = 20;

// Canny Thresholds:
int Controls::canny_1;
int Controls::canny_2;
int Controls::canny_3 = 3;

vector<Vec4i> Controls::lines;

void Controls::loadNextImage()
{
    // images setup:
    Globals::img_idx = (Globals::img_idx + 1) % Globals::imagePaths.size();

    Globals::img = Globals::nextImg;                                // load image
    Globals::nextImg.load(Globals::imagePaths.at((Globals::img_idx + 1) % Globals::imagePaths.size())); // load next image already
    Globals::img.resize(IMAGE_WIDTH, IMAGE_HEIGHT);                                         // ATTENTION: IMAGE_WIDTH AND IMAGE_HEIGHT DO AFFECT THE SCANNING. TODO: RESIZE ONLY SHORTLY BEFORE DRAWING (USING ANOTHER INSTANCE)

    Globals::scaledImage.loadImage(Globals::imagePaths.at(Globals::img_idx));
    Globals::scaledImage.resize(IMAGE_WIDTH * Globals::image_scaling, IMAGE_HEIGHT * Globals::image_scaling);

    Globals::colorImg.allocate(Globals::img.getWidth(), Globals::img.getHeight());
    Globals::grayImg.allocate(Globals::img.getWidth(), Globals::img.getHeight());

    Globals::colorImg.setFromPixels(Globals::img.getPixels());
    Globals::colorImg.convertToGrayscalePlanarImage(Globals::grayImg, 0);
    Globals::grayImg.threshold(Controls::img_thresholdLow, Controls::img_thresholdHigh);

    // edge detection:
    Canny(Globals::grayImg, Globals::edge_img, Controls::canny_1, Controls::canny_2, Controls::canny_3);
    Sobel(Globals::grayImg, Globals::sobel_img);
    Globals::sobel_img.update();
    Globals::edge_img.update();

    ofLogNotice("loading image " + Globals::imagePaths.at(Globals::img_idx));

    Controls::doQuickScanNextUpdate = true;
}