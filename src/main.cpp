#include "ofMain.h"
#include "ofApp.h"
#include "scanner.h"

//========================================================================
int main( ){

	ofSetupOpenGL(1630, 1080, OF_WINDOW);			// <-------- setup the GL context

	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofRunApp( new ofApp());

}