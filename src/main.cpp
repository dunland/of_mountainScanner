#include "ofMain.h"
#include "ofApp.h"
#include "Globals.h"
#include "consoleApp.h"

//========================================================================
int main( ){

	ofGLWindowSettings settings;
	settings.setSize(1024, 768);
	settings.setPosition(ofVec2f(300, 0));
	auto mainWindow = ofCreateWindow(settings);

	settings.setSize(300, 300);
	settings.setPosition(ofVec2f(0, 0));
	auto consoleWindow = ofCreateWindow(settings);

	auto mainApp = make_shared<ofApp>();
	auto consoleApp_ = make_shared<consoleApp>();

	ofRunApp(consoleWindow, consoleApp_);
	ofRunApp(mainWindow, mainApp);
	ofRunMainLoop();

}
