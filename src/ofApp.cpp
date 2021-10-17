#include "ofApp.h"
#include "Globals.h"

using namespace ofxCv;
using namespace cv;

//--------------------------------------------------------------
void ofApp::setup()
{
    ofSetWindowTitle("ofApp");

    ofSetVerticalSync(true);

    img.load(images[img_idx]);
    img.resize(IMAGE_WIDTH, IMAGE_HEIGHT);

    colorImg.allocate(img.getWidth(), img.getHeight());
    grayImg.allocate(img.getWidth(), img.getHeight());
    colorImg.setFromPixels(img.getPixels());
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
    // TODO: make gui global and use gui_lowerRidgeSlider.mouseReleased()
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

    if (Scanner::scan_iteration >= Scanner::maxIterations)
    {
        img_idx = (img_idx + 1) % 3;

        // images setup:
        img.load(images[img_idx]);
        img.resize(IMAGE_WIDTH, IMAGE_HEIGHT);
        colorImg.allocate(img.getWidth(), img.getHeight());
        grayImg.allocate(img.getWidth(), img.getHeight());
        colorImg.setFromPixels(img.getPixels());
        colorImg.convertToGrayscalePlanarImage(grayImg, 0);

        // edge detection:
        Canny(grayImg, edge_img, Controls::canny_1, Controls::canny_2, Controls::canny_3);
        Sobel(grayImg, sobel_img);
        sobel_img.update();
        edge_img.update();

        Scanner::getMinMax(edge_img.getPixels());

        Scanner::scan_iteration = 0;
        cout << "loading image " + images[img_idx] << endl;
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