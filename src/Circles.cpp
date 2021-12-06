#include "Circles.h"
#include "ofMain.h"

void Circle::draw()
{
    ofSetColor(255);
    ofFill();
    ofDrawCircle(xpos, ypos, radius);
}

void Circle::update_radius()
{
    radius -= CIRCLES_SHRINK_SPEED;
    if (radius <= 0)
        bKillMe = true;
}
