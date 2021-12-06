#include "Circles.h"
#include "ofMain.h"

void Circle::draw(bool flyOff)
{
    ofSetColor(255);
    ofFill();
    ofDrawCircle(xpos, ypos, radius);

    if (flyOff)
    {
        float r = (ofRandom(0, 1) >= 0.1) ? 1 : -1;
        ypos += ofNoise(ypos) * r;
    }
}

void Circle::update_radius(float shrink_speed)
{
    radius -= shrink_speed;
    if (radius <= 0)
        bKillMe = true;
}
