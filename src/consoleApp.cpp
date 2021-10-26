#include "consoleApp.h"
#include <iostream>
#include "Globals.h"

//--------------------------------------------------------------
void consoleApp::setup()
{
    ofSetWindowTitle("consoleApp");

    gui.setup();
    gui.add(gui_imgThresholdLow.setup("Image Low Threshold", 113, 0, 255));
    // gui.add(gui_imgThresholdHigh.setup("Image High Threshold", 0, 0, 1));
    // gui.add(gui_canny_1.setup("Canny Threshold 1", 0, 0, 255));
    // gui.add(gui_canny_2.setup("Canny Threshold 2", 0, 0, 255));
    // gui.add(gui_edgeThreshold.setup("Edge Threshold", 50, 0, 100));
    // gui.add(gui_lineThreshold.setup("Line Threshold", 0, 0, 200));
    // gui.add(gui_minLineLength.setup("min line length", 0, 0, 15));
    // gui.add(gui_maxLineGap.setup("max line gap", 20, 0, 20));
    gui.add(gui_upperRidgeLimit.setup("upper ridge limit", IMAGE_HEIGHT / 4, 0, IMAGE_HEIGHT));
    gui.add(gui_oscillationCenter.setup("oscillation center", IMAGE_HEIGHT / 2, 0, IMAGE_HEIGHT));
    gui.add(gui_lowerRidgeLimit.setup("lower ridge limit", IMAGE_HEIGHT * 3 / 4, IMAGE_HEIGHT, 0));
    gui.add(gui_scanningSpeed.setup("scanning speed", SCANNING_SPEED, 1, 10));
    gui.add(gui_scanModeButton.setup("set scan mode", 8, 0, 15));
    gui.add(gui_send_button.setup("quickScan and send compiled data", 8, 0, 15));
    gui.setPosition(0, 120);

    // communication with Pd
    Communication::sender.setup(HOST, SENDING_PORT);
    // send 1 to activate udp:
    ofxOscMessage m;
    m.setAddress("/setup/");
    m.addIntArg(1);
    Communication::sender.sendMessage(m, false);

    ofSleepMillis(1);

    Communication::receiver.setup(RECEIVING_PORT);
}

//--------------------------------------------------------------
void consoleApp::update()
{
    // update gui variables:
    Controls::img_thresholdLow = gui_imgThresholdLow;   // img threshold
    // Controls::img_thresholdHigh = gui_imgThresholdHigh; // img threshold
    // Controls::canny_1 = gui_canny_1;                    // img threshold
    // Controls::canny_2 = gui_canny_2;                    // img threshold
    // Controls::edgeThreshold = gui_edgeThreshold;        // line detection
    // Controls::lineThreshold = gui_lineThreshold;        // line detection
    // Controls::minLineLength = gui_minLineLength;        // line detection
    // Controls::maxLineGap = gui_maxLineGap;              // line detection

    // TODO: quickScan whenever one of these was changed (via gui.mouseRelease)
    Scanner::oscillationCenter = gui_oscillationCenter; // upper scan area
    Scanner::upperRidgeLimit = gui_upperRidgeLimit;     // upper scan area
    Scanner::lowerRidgeLimit = gui_lowerRidgeLimit;     // lower scan area
    Scanner::scanning_speed = gui_scanningSpeed;

    if (gui_send_button)
    {
        Scanner::quickScan_relative(Globals::edge_img.getPixels());
        gui_send_button = false;
    }

    Scanner::scan_mode = (gui_scanModeButton) ? Relative : Absolute;

    if (gui_mouse_released)
    {
        // ---------------- observe limits changes -----------------------
        // TODO: use gui_lowerRidgeSlider.mouseReleased()
        static int prev_lowRidgeLimit = Scanner::lowerRidgeLimit;
        if (prev_lowRidgeLimit != Scanner::lowerRidgeLimit)
        {
            Controls::doQuickScanNextUpdate = true;
            prev_lowRidgeLimit = Scanner::lowerRidgeLimit;
            cout << "lowerRidgeLimit changed! new limit at " << Scanner::lowerRidgeLimit << " <-- ymax at" << Scanner::ymax << endl;
        }

        static int prev_highRidgeLimit = Scanner::upperRidgeLimit;
        if (prev_highRidgeLimit != Scanner::upperRidgeLimit)
        {
            Controls::doQuickScanNextUpdate = true;
            prev_highRidgeLimit = Scanner::upperRidgeLimit;
            cout << "upperRidgeLimit changed! new limit at " << Scanner::upperRidgeLimit << " <-- ymin at" << Scanner::ymin << endl;
        }

        static int prev_oscillationCenter = Scanner::oscillationCenter;
        if (prev_oscillationCenter != Scanner::oscillationCenter)
        {
            Controls::doQuickScanNextUpdate = true;
            prev_oscillationCenter = Scanner::oscillationCenter;
            cout << "oscillationCenter changed! " << Scanner::oscillationCenter << " <-- oscillating at" << Scanner::oscillationCenter << endl;
        }

        static int prev_imgThreshold = Controls::img_thresholdLow;
        if (prev_imgThreshold != Controls::img_thresholdLow)
        {
            Controls::doQuickScanNextUpdate = true;
            prev_imgThreshold = Controls::img_thresholdLow;
            cout << "img threshold changed! " << Controls::img_thresholdLow << " <-- now at" << Controls::img_thresholdLow << endl;
        }
        gui_mouse_released = false;
    }

    // ------------------------ communication -------------------------
    // ofxOscMessage incoming_message;
    // Communication::receiver.getNextMessage(incoming_message);

    // if (incoming_message.getAddress() == "/scanner/pos")
    // {
    //     Scanner::x_pos = incoming_message.getArgAsInt(0);
    //     cout << "incoming message at " << incoming_message.getAddress() << ": " << incoming_message.getArgAsInt(0) << endl;
    // }
}

//--------------------------------------------------------------
void consoleApp::draw()
{
    ofBackground(0);

    // display parameters:
    string c3Str = "canny_3 [c]: " + ofToString(Controls::canny_3);
    ofDrawBitmapString(c3Str, 20, 10);

    string draw_modes[5] = {"", "color", "grey", "sobel", "edge"};
    string drawStr = "draw_mode [1,2,3,4]: " + draw_modes[Controls::draw_mode];
    ofDrawBitmapString(drawStr, 20, 30);

    string fpsStr = "fps: " + ofToString(ofGetFrameRate());
    ofDrawBitmapString(fpsStr, 20, 50);

    string scannerPosStr = "scanner_pos: " + ofToString(Scanner::x_pos);
    ofDrawBitmapString(scannerPosStr, 20, 70);

    if (Scanner::scan_mode == Absolute)
    {
        string scanModeStr = "scan mode: Absolute";
        ofDrawBitmapString(scanModeStr, 20, 90);
    }
    else
    {
        string scanModeStr = "scan mode: Relative";
        ofDrawBitmapString(scanModeStr, 20, 90);
    }

    string currentImgStr = "image: " + Globals::images[Globals::img_idx];
    ofDrawBitmapString(currentImgStr, 20, 110);

    gui.draw();
}

//--------------------------------------------------------------
void consoleApp::keyPressed(int key)
{
}

//--------------------------------------------------------------
void consoleApp::keyReleased(int key)
{
    if (key == '1')
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
void consoleApp::mouseMoved(int x, int y)
{
}

//--------------------------------------------------------------
void consoleApp::mouseDragged(int x, int y, int button)
{
}

//--------------------------------------------------------------
void consoleApp::mousePressed(int x, int y, int button)
{
}

//--------------------------------------------------------------
void consoleApp::mouseReleased(int x, int y, int button)
{
    gui_mouse_released = true;
}

//--------------------------------------------------------------
void consoleApp::mouseEntered(int x, int y)
{
}

//--------------------------------------------------------------
void consoleApp::mouseExited(int x, int y)
{
}

//--------------------------------------------------------------
void consoleApp::windowResized(int w, int h)
{
}

//--------------------------------------------------------------
void consoleApp::gotMessage(ofMessage msg)
{
}

//--------------------------------------------------------------
void consoleApp::dragEvent(ofDragInfo dragInfo)
{
}