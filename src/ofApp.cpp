#include "ofApp.h"
#include "Globals.h"

using namespace ofxCv;
using namespace cv;

//--------------------------------------------------------------
void ofApp::setup()
{

    ofSetVerticalSync(true);

    img.load("mountain_1920x1080.JPG");
    // img.resize(img.getWidth() / 3, img.getHeight() / 3);
    ofPixels &img_pix = img.getPixels();

    colorImg.allocate(img.getWidth(), img.getHeight());
    grayImg.allocate(img.getWidth(), img.getHeight());
    colorImg.setFromPixels(img_pix);
    colorImg.convertToGrayscalePlanarImage(grayImg, 0);
}

//--------------------------------------------------------------
void ofApp::update()
{

    ofPixels &img_pix = img.getPixels();

    colorImg.convertToGrayscalePlanarImage(grayImg, 0); // reset grayImg to be updated from scratch in next step
    grayImg.threshold(Controls::img_threshold);

    // edge detection:
    Canny(grayImg, edge_img, Controls::canny_1, Controls::canny_2, Controls::canny_3);
    Sobel(grayImg, sobel_img);
    sobel_img.update();
    edge_img.update();

    // --- update min/max Scanner values if limits change -----
    static int prev_lowRidgeLimit = Scanner::lowerRidgeLimit;
    if (prev_lowRidgeLimit != Scanner::lowerRidgeLimit)
    {
        Scanner::getMinMax(edge_img.getPixels());
        prev_lowRidgeLimit = Scanner::lowerRidgeLimit;
        cout << "lowerRidgeLimit changed! new limit at " << Scanner::lowerRidgeLimit << " <-- ymax at" << Scanner::ymax << endl;
    }

    static int prev_highRidgeLimit = Scanner::upperRidgeLimit;
    if (prev_highRidgeLimit != Scanner::upperRidgeLimit)
    {
        Scanner::getMinMax(edge_img.getPixels());
        prev_highRidgeLimit = Scanner::upperRidgeLimit;
        cout << "upperRidgeLimit changed! new limit at " << Scanner::upperRidgeLimit << " <-- ymin at" << Scanner::ymin << endl;
    }

    // ------------ perform the scanning -----------------------
    if (Scanner::scanning)
    {
        switch (Scanner::scan_mode)
        {
        case Absolute:
            Scanner::scan_absolute(edge_img.getPixels());
            break;
        case Relative:
            Scanner::scan_relative(edge_img.getPixels());
            break;

        default:
            break;
        }
    }
}

//--------------------------------------------------------------
void ofApp::draw()
{
    ofBackground(0);

    // draw images:
    ofSetColor(255);

    switch (Controls::draw_mode)
    {
    case 1:
        img.draw(0, 0);
        break;
    case 2:
        grayImg.draw(0, 0);
        break;
    case 3:
        sobel_img.draw(0, 0);
        break;
    case 4:
        edge_img.draw(0, 0);
        break;
    default:
        break;
    }

    // line detection:
    Mat mat = toCv(edge_img);

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
    }
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y)
{
    // mouse_x = ofGetMouseX();
    // cout << mouse_x << endl;
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