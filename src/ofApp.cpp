/*
mountainScanner
by David Unland

The app will scan all ".jpg" images in the ./bin/data folder.
Settings, like number of scans until  are to be set in settings.h
*/

#include "ofApp.h"
#include "Globals.h"

using namespace ofxCv;
using namespace cv;

////////////////////////////////// SETUP //////////////////////////////
void ofApp::setup()
{
    ofSetWindowTitle("mountainScanner");

    ofSetVerticalSync(true);

    // setup images
    ofDirectory dir(""); // read folder ./bin/data
    dir.allowExt("jpg");
    dir.listDir();
    dir = dir.getSorted();
    for (int i = 0; i < dir.size(); i++)
        Globals::images.push_back(dir.getPath(i));

    Globals::img.loadImage(Globals::images.at(Globals::img_idx));
    Globals::img.resize(IMAGE_WIDTH, IMAGE_HEIGHT);

    Globals::scaledImage.loadImage(Globals::images.at(Globals::img_idx));
    Globals::scaledImage.resize(IMAGE_WIDTH * Globals::image_scaling, IMAGE_HEIGHT * Globals::image_scaling);

    Globals::colorImg.allocate(IMAGE_WIDTH, IMAGE_HEIGHT);
    Globals::grayImg.allocate(IMAGE_WIDTH, IMAGE_HEIGHT);
    Globals::colorImg.setFromPixels(Globals::img.getPixels());
    Globals::colorImg.convertToGrayscalePlanarImage(Globals::grayImg, 0);
    Globals::grayImg.threshold(Controls::img_thresholdLow, Controls::img_thresholdHigh);

    Controls::doQuickScanNextUpdate = true;
}

////////////////////////////////// UPDATE /////////////////////////////
void ofApp::update()
{

 // OSC (via nanokorg) --------------------------------------
    static int channel;
    static int value;
    while (receiver.hasWaitingMessages())
    {
        ofxOscMessage message;
        receiver.getNextMessage(message);

        if (message.getAddress() == "/midi/channel/")
        {
            ofLogNotice(message.getAddress());
            ofLogNotice(ofToString(message.getArgAsInt(0)));
            channel = message.getArgAsInt(0);
        }
        else if (message.getAddress() == "/midi/value/")
        {
            ofLogNotice(message.getAddress());
            ofLogNotice(ofToString(message.getArgAsInt(0)));
            value = message.getArgAsInt(0);
        }
        // midiParams[channel] = value;

        if(channel == 0) // upper
        {
            Scanner::upperRidgeLimit = int(value * IMAGE_HEIGHT / 128);
        }
        else if(channel == 1) // center
        {
            Scanner::oscillationCenter = int(value * IMAGE_HEIGHT / 128);
        }
        else if(channel == 1) // lower
        {
            Scanner::lowerRidgeLimit = int(value * IMAGE_HEIGHT / 128);
        }

        ofLogNotice(ofToString(message.getAddress()));
        ofLogNotice(ofToString(message));
    }

    // --------- load next image upon request: ------------------------
    if (Controls::doLoadNextImage)
    {
        Controls::loadNextImage();
        Controls::doLoadNextImage = false;
    }

    // ---------------------- IMAGE CONVERSIONS -----------------------
    Globals::colorImg.convertToGrayscalePlanarImage(Globals::grayImg, 0); // reset grayImg to be updated from scratch in next step
    Globals::grayImg.threshold(Controls::img_thresholdLow, Controls::img_thresholdHigh);

    // edge detection:
    Canny(Globals::grayImg, Globals::edge_img, Controls::canny_1, Controls::canny_2, Controls::canny_3);
    Sobel(Globals::grayImg, Globals::sobel_img);
    Globals::sobel_img.update();
    Globals::edge_img.update();

    Globals::scaledEdgeImage = Globals::edge_img;
    Globals::scaledEdgeImage.resize(IMAGE_WIDTH * Globals::image_scaling, IMAGE_HEIGHT * Globals::image_scaling);

    // ------------------- perform the scanning -----------------------
    if (Scanner::scanning)
    {
        switch (Scanner::scan_mode)
        {
        case Absolute:
            Scanner::scan_absolute(Globals::edge_img.getPixels());
            break;
        case Relative:
            Scanner::scan_relative(Globals::edge_img.getPixels());
            break;

        default:
            break;
        }
    }

    if (Controls::doQuickScanNextUpdate && ofGetFrameNum() > 10)
    {
        Scanner::quickScan_relative(Globals::edge_img.getPixels());
        Controls::doQuickScanNextUpdate = false;
    }
}

/////////////////////////////////// DRAW //////////////////////////////
void ofApp::draw()
{
    ofBackground(0);

    // draw images:
    ofSetColor(255);

    if (Globals::image_scaling != 1)
    {
        Globals::scaledImage.draw(0,0);
        Globals::scaledEdgeImage.draw(0,IMAGE_HEIGHT * Globals::image_scaling);
    }
    else
        switch (Controls::draw_mode)
        {
        case 1:
            Globals::img.draw(0, 0);
            break;
        case 2:
            Globals::grayImg.draw(0, 0);
            break;
        case 3:
            Globals::sobel_img.draw(0, 0);
            break;
        case 4:
            Globals::edge_img.draw(0, 0);
            break;
        default:
            break;
        }

    // line detection:
    Mat mat = toCv(Globals::edge_img);

    if (Controls::do_edgeDetection)
    {
        HoughLinesP(mat, Controls::lines, 120, CV_PI / 180, Controls::lineThreshold, Controls::minLineLength, Controls::maxLineGap);

        ofSetColor(255, 0, 0);
        for (int i = 0; i < Controls::lines.size(); i++)
        {
            float x1 = Controls::lines[i][0];
            float y1 = Controls::lines[i][1];
            float x2 = Controls::lines[i][2];
            float y2 = Controls::lines[i][3];
            ofPolyline l;
            l.addVertex(x1, y1);
            l.addVertex(x2, y2);

            l.draw();
        }
    }

    Scanner::draw();

    if (Scanner::do_draw_limits)
        Scanner::drawRidgeLimits();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key)
{
    if (key == 'f')
    {
        ofToggleFullscreen();
        Globals::image_scaling = (Globals::image_scaling == 0.5) ? 1 : 0.5;
        Globals::scaledImage.resize(IMAGE_WIDTH * Globals::image_scaling, IMAGE_HEIGHT * Globals::image_scaling);
    }
    else if (key == '1')
    {
        Controls::draw_mode = 1;
    }
    else if (key == '2')
    {
        Controls::draw_mode = 2;
    }
    else if (key == '3')
    {
        Controls::draw_mode = 3;
    }
    else if (key == '4')
    {
        Controls::draw_mode = 4;
    }
    else if (key == '0')
    {
        Controls::draw_mode = 0;
    }

    else if (key == 'c')
    {
        if (Controls::canny_3 == 3)
            Controls::canny_3 = 5;
        else if (Controls::canny_3 == 5)
            Controls::canny_3 = 7;
        else if (Controls::canny_3 == 7)
            Controls::canny_3 = 3;
    }

    else if (key == 'l')
        Scanner::do_draw_limits = !Scanner::do_draw_limits;

    else if (key == ' ')
    {
        Scanner::scanning = !Scanner::scanning;
    }

    else if (key == OF_KEY_RETURN)
    {
        Controls::doLoadNextImage = true;
    }
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y)
{
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button)
{
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button)
{
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button)
{
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y)
{
}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y)
{
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h)
{
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg)
{
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo)
{
}