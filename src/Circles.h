#pragma once

#include "ofMain.h"
#include "settings.h"

class Circle
{
public:

    Circle(int x_pos, int y_pos, float radius_)
    {
        xpos = x_pos;
        ypos = y_pos;
        radius = radius_;
    }


    int xpos, ypos;
    float radius = CIRCLES_CREATION_STEP;
    bool bKillMe = false;

    void draw(bool circlesFlyOff);

    void update_radius(float shrinkSpeed);

    ~Circle(){
        delete(this);
    }
};