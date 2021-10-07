#include "ofApp.h"
#include "Globals.h"

using namespace ofxCv;
using namespace cv;

//--------------------------------------------------------------
void ofApp::setup()
{

    ofSetVerticalSync(true);

    img.load("mountain_1920x1080.JPG");
    img.resize(img.getWidth() / 3, img.getHeight() / 3);
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

    Scanner::x_pos = (Scanner::x_pos + 1) % int(img.getWidth());

    colorImg.convertToGrayscalePlanarImage(grayImg, 0); // reset grayImg to be updated from scratch in next step
    grayImg.threshold(Controls::img_threshold);

    // edge detection:
    Canny(grayImg, edge_img, Controls::canny_1, Controls::canny_2, 3);
    Sobel(grayImg, sobel_img);
    sobel_img.update();
    edge_img.update();
}

//--------------------------------------------------------------
void ofApp::draw()
{
    ofBackground(0);

    // draw images:
    ofSetColor(255);

    img.draw(0, 0);
    grayImg.draw(0, img.getHeight());
    sobel_img.draw(img.getWidth(), 0);
    edge_img.draw(img.getWidth(), img.getHeight());

    // line detection:
    Mat mat = toCv(edge_img);

    HoughLinesP(mat, Controls::lines, 2, CV_PI / 180, Controls::lineThreshold, 15, 20);
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